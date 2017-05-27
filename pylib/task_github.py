#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that these additional
# terms apply under section 7:
#
#   No right, title or interest in or to any trade mark, service mark, logo or
#   trade name of of National ICT Australia Limited, ABN 62 102 206 173
#   ("NICTA") or its licensors is granted. Modified versions of the Program
#   must be plainly marked as such, and must not be distributed using
#   "eChronos" as a trade mark or product name, or misrepresented as being the
#   original Program.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# @TAG(NICTA_AGPL)
#

from os import environ
from github import Github


def render_pr(pull_req):
    rmd = "{}{}{}".format(render_header(pull_req), render_reviews(pull_req), render_comments(pull_req))
    print(rmd)


def render_header(pull_req):
    fmt = """# [{}]({})

{}

_By [{}](mailto:{})_
_Base: revision [{}]({}/commits/{}) in [{}]({}/tree/{})_
_Head: revision [{}]({}/commits/{}) in [{}]({}/tree/{})_

"""
    return fmt.format(pull_req.title, pull_req.html_url,
                      pull_req.body,
                      pull_req.user.name, pull_req.user.email,
                      pull_req.base.sha, pull_req.html_url, pull_req.base.sha, pull_req.base.label,
                      pull_req.base.repo.html_url, pull_req.base.ref,
                      pull_req.head.sha, pull_req.html_url, pull_req.head.sha, pull_req.head.label,
                      pull_req.head.repo.html_url, pull_req.head.ref)


def render_reviews(pull_req):
    reviews = []
    for review in pull_req.get_reviews():
        reviews.append(review)

    if reviews:
        rmd = "# Reviews\n\n"
        for rvw in reviews:
            rmd += "**[{}]({})**  \n".format(rvw.state, rvw.html_url)
            rmd += "_Author: [{}](mailto:{})_  \n".format(rvw.user.name, rvw.user.email)
            rmd += "_Revision: [{}]({}/{})_  \n".format(rvw.commit_id, pull_req.commits_url, rvw.commit_id)
            # pylint: disable=protected-access
            rmd += "_Date: {}_  \n".format(rvw._rawData["submitted_at"].replace("T", " ").rstrip("Z"))
            rmd += "> {}\n\n---\n\n".format(rvw.body.replace("\n", "\n> "))
    else:
        rmd = ""

    return rmd


def render_comments(pull_req):
    comments = []
    for comment in pull_req.get_review_comments():
        comments.append(comment)

    for comment in pull_req.get_issue_comments():
        comments.append(comment)

    if comments:
        rmd = "# Comments\n\n"
        for cmt in sorted(comments, key=lambda x: x.updated_at):
            if hasattr(cmt, "position"):
                rmd += "_File {}:{}_  \n".format(cmt.path, cmt.position)
                rmd += "_Revision {}_  \n".format(cmt.commit_id)
            rmd += "_Author: [{}](mailto:{})_  \n".format(cmt.user.name, cmt.user.email)
            rmd += "_Date: {}_  \n".format(cmt.updated_at)
            rmd += "_Link: {}_  \n".format(cmt.html_url)
            rmd += "> {}\n\n---\n\n".format(cmt.body.replace("\n", "\n> "))
    else:
        rmd = ""

    return rmd


if __name__ == "__main__":
    GITHUB = Github("stgstgstg", environ['GITHUB_PASSWORD'])

    for repo in GITHUB.get_user().get_repos():
        for pull in repo.get_pulls():
            render_pr(pull)
