# Copyright (C) 2009 Google Inc. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from webkitpy.networktransaction import NetworkTransaction
from webkitpy.thirdparty.autoinstalled import Browser
from webkitpy.webkit_logging import log

# WebKit includes a built copy of BeautifulSoup in Scripts/webkitpy/thirdparty
# so this import should always succeed.
from webkitpy.thirdparty.BeautifulSoup import BeautifulSoup

import urllib2


class StatusServer:
    default_host = "webkit-commit-queue.appspot.com"

    def __init__(self, host=default_host):
        self.set_host(host)
        self.browser = Browser()

    def set_host(self, host):
        self.host = host
        self.url = "http://%s" % self.host

    def results_url_for_status(self, status_id):
        return "%s/results/%s" % (self.url, status_id)

    def _add_patch(self, patch):
        if not patch:
            return
        if patch.bug_id():
            self.browser["bug_id"] = str(patch.bug_id())
        if patch.id():
            self.browser["patch_id"] = str(patch.id())

    def _add_results_file(self, results_file):
        if not results_file:
            return
        self.browser.add_file(results_file, "text/plain", "results.txt", 'results_file')

    def _post_status_to_server(self, queue_name, status, patch, results_file):
        if results_file:
            # We might need to re-wind the file if we've already tried to post it.
            results_file.seek(0)

        update_status_url = "%s/update-status" % self.url
        self.browser.open(update_status_url)
        self.browser.select_form(name="update_status")
        self.browser["queue_name"] = queue_name
        self._add_patch(patch)
        self.browser["status"] = status
        self._add_results_file(results_file)
        return self.browser.submit().read() # This is the id of the newly created status object.

    def _post_svn_revision_to_server(self, svn_revision_number, broken_bot):
        update_svn_revision_url = "%s/update-svn-revision" % self.url
        self.browser.open(update_svn_revision_url)
        self.browser.select_form(name="update_svn_revision")
        self.browser["number"] = str(svn_revision_number)
        self.browser["broken_bot"] = broken_bot
        return self.browser.submit().read()

    def update_status(self, queue_name, status, patch=None, results_file=None):
        log(status)
        return NetworkTransaction().run(lambda: self._post_status_to_server(queue_name, status, patch, results_file))

    def update_svn_revision(self, svn_revision_number, broken_bot):
        log("SVN revision: %s broke %s" % (svn_revision_number, broken_bot))
        return NetworkTransaction().run(lambda: self._post_svn_revision_to_server(svn_revision_number, broken_bot))

    def _fetch_url(self, url):
        try:
            return urllib2.urlopen(url).read()
        except urllib2.HTTPError, e:
            if e.code == 404:
                return None
            raise e

    def patch_status(self, queue_name, patch_id):
        patch_status_url = "%s/patch-status/%s/%s" % (self.url, queue_name, patch_id)
        return self._fetch_url(patch_status_url)

    def svn_revision(self, svn_revision_number):
        svn_revision_url = "%s/svn-revision/%s" % (self.url, svn_revision_number)
        return self._fetch_url(svn_revision_url)
