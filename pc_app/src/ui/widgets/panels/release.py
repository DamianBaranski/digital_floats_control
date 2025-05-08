import requests
import urllib.request

class Release:
    def __init__(self, owner, repo):
        self.url = f"https://api.github.com/repos/{owner}/{repo}/releases/latest"

    def getLatestTag(self):
        response = requests.get(self.url)
        return response.json()["tag_name"]

    def downloadLatestAsset(self, filename):
        response = requests.get(self.url)
        download_url = None
        for asset in response.json()["assets"]:
            if filename == asset["name"]:
                download_url = asset["browser_download_url"]
        if download_url:
            urllib.request.urlretrieve(download_url, filename)
            return True
        else:
            return False


