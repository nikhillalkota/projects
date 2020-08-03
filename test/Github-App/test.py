import os
from flask import Flask
from flask_githubapp import GitHubApp

app = Flask(__name__)

app.config['GITHUBAPP_ID'] = int(73792)

with open(os.environ['GITHUBAPP_KEY_PATH'], 'rb') as key_file:
    app.config['GITHUBAPP_KEY'] = key_file.read()

app.config['GITHUBAPP_SECRET'] = os.environ['GITHUB_SECRET_APP_WEBHOOK']

github_app = GitHubApp(app)


@github_app.on('pull_request.opened')
def cruel_closer():
    # owner = github_app.payload['repository']['owner']['login']
    # repo = github_app.payload['repository']['name']
    # num = github_app.payload['issue']['number']
    issue = github_app.installation_client.issue('{body:Hey @${user} 👋, Thanks for the PR !!! You are Awesome.}')
    issue.create_comment('Could not replicate.')
    issue.close()