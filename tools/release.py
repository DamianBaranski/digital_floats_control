import requests
import os
import json
import sys

# GitHub access token and repository details
GITHUB_TOKEN = sys.argv[1]
REPO_OWNER = sys.argv[2]
REPO_NAME = sys.argv[3]
TAG_NAME = sys.argv[4]

# Headers for the API requests
headers = {
    'Authorization': f'token {GITHUB_TOKEN}',
    'Accept': 'application/vnd.github+json'
}

def create_release(tag_name, release_name, release_body, draft=False, prerelease=False):
    """
    Create a new GitHub release.
    
    :param tag_name: The tag for the release (e.g., 'v1.0.0').
    :param release_name: The name of the release.
    :param release_body: Description of the release.
    :param draft: Whether the release is a draft (default is False).
    :param prerelease: Whether the release is a prerelease (default is False).
    :return: The release ID if the release is created successfully, otherwise None.
    """
    create_release_url = f'https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/releases'
    release_data = {
        'tag_name': tag_name,
        'name': release_name,
        'body': release_body,
        'draft': draft,
        'prerelease': prerelease
    }

    response = requests.post(create_release_url, headers=headers, data=json.dumps(release_data))
    
    if response.status_code == 201:
        print('Release created successfully.')
        release_info = response.json()
        return release_info['id']
    else:
        print(f'Failed to create release. Response: {response.status_code}, {response.text}')
        return None

def upload_asset(release_id, file_path):
    """
    Upload an asset (file) to an existing GitHub release.
    
    :param release_id: The ID of the release to upload the asset to.
    :param file_path: The local path to the file being uploaded.
    :return: None
    """
    file_name = os.path.basename(file_path)
    upload_url = f'https://uploads.github.com/repos/{REPO_OWNER}/{REPO_NAME}/releases/{release_id}/assets?name={file_name}'
    
    # Read the file and upload it
    with open(file_path, 'rb') as file_data:
        upload_headers = {
            'Authorization': f'token {GITHUB_TOKEN}',
            'Content-Type': 'application/octet-stream'
        }
        response = requests.post(upload_url, headers=upload_headers, data=file_data)
    
    if response.status_code == 201:
        print(f'Successfully uploaded {file_name} to the release.')
    else:
        print(f'Failed to upload {file_name}. Response: {response.status_code}, {response.text}')

def list_releases():
    """
    List all releases in the repository.
    
    :return: List of releases sorted by creation date (oldest to newest).
    """
    list_releases_url = f'https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/releases'
    response = requests.get(list_releases_url, headers=headers)
    
    if response.status_code == 200:
        releases = response.json()
        # Sort releases by creation date (oldest first)
        sorted_releases = sorted(releases, key=lambda x: x['created_at'])
        return sorted_releases
    else:
        print(f'Failed to list releases. Response: {response.status_code}, {response.text}')
        return []

def delete_release(release_id):
    """
    Delete a GitHub release by its ID.
    
    :param release_id: The ID of the release to delete.
    :return: None
    """
    delete_url = f'https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/releases/{release_id}'
    response = requests.delete(delete_url, headers=headers)
    
    if response.status_code == 204:
        print(f'Successfully deleted release with ID {release_id}.')
    else:
        print(f'Failed to delete release {release_id}. Response: {response.status_code}, {response.text}')

def remove_old_releases(keep_last=3):
    """
    Remove old releases from the repository, keeping only the last `keep_last` releases.
    
    :param keep_last: Number of most recent releases to keep.
    :return: None
    """
    releases = list_releases()
    
    if len(releases) > keep_last:
        # Releases to delete are those beyond the most recent `keep_last`
        releases_to_delete = releases[:-keep_last]
        
        for release in releases_to_delete:
            release_id = release['id']
            print(f'Deleting release: {release["name"]} (ID: {release_id})')
            delete_release(release_id)
    else:
        print(f'No need to delete releases. You have {len(releases)} releases, which is less than or equal to {keep_last}.')

def main():
    # Define release details
    RELEASE_NAME = 'Release ' + TAG_NAME
    RELEASE_BODY = ''
    
    # Step 1: Remove old releases, keep only the last 3
    remove_old_releases(keep_last=3)

    # Step 2: Create a new release
    release_id = create_release(TAG_NAME, RELEASE_NAME, RELEASE_BODY)
    
    if release_id:
        # Step 3: Upload the file to the newly created release
        upload_asset(release_id, "build/application/floats_bs.bin")
        upload_asset(release_id, "build/bootloader/bootloader_bs.bin")
        upload_asset(release_id, "build/dist/DigitalFloatsControl")
        #upload_asset(release_id, "build/dist/DigitalFloatsControl.exe")



if __name__ == '__main__':
    main()

