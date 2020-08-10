from flask import Flask, request, make_response
import hmac,json
import os
from hmac import HMAC, compare_digest
from github import Github
import hashlib
from octokit import Octokit

app = Flask(__name__)


@app.route('/')
def hello():
    return "Hello World!"


def is_valid_request(event_body, header_send):
    signature = os.environ['GITHUB_SECRET_APP_WEBHOOK']
    expected_sign = HMAC(key=signature.encode(), msg=event_body, digestmod=hashlib.sha1).hexdigest()
    final_hash = 'sha1=' + expected_sign

    if hmac.compare_digest(final_hash, header_send):
        print('validation passed')
        return True
    else:
        print("Validation Failed")
        return False

@app.route('/event', methods=['POST'])
def event():

    event_body = request.get_data()
    # print(event_body)
    headers = dict(request.headers)
    header_send = request.headers.get('X-Hub-Signature')
    print(is_valid_request(event_body, header_send))
    # print(headers)
    # print(event_body)
    event_body = event_body.decode('utf-8')
    request_parse = json.loads(event_body)


    # octokit = Octokit(auth='app', app_id='73792', private_key=os.environ['GITHUB_SECRET_APP_WEBHOOK'])


    g = Github(os.environ['GITHUB_COMMIT_STATUS_TOKEN'])

    repo = g.get_repo("nikhillalkota/projects")
    print(repo.name)
    sha = request_parse["pull_request"]["head"]["sha"]
    commit = repo.get_commit(sha=sha)
    print(commit.commit.author.date)
    repo.get_commit(sha=sha).create_status(state="started",target_url="https://FooCI.com",description="FooCI is building",context="ci/FooCI")
    return make_response("", 200)



if __name__ == '__main__':
    app.run(debug=True)