#!/usr/bin/env python3.7
#
# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved. RTI
# grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose. RTI
# is under no obligation to maintain or support the Software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.
"""Manage RTI ConnextDDS versions in AWS S3 Bucket.

This module implements some functions to upload/delete/list RTI ConnextDDS
versions from an AWS S3 bucket. If you have the credentials in a file located
in a non-default location (~/.aws/config), you can specify it as an argument in
the command line (-c <path_to_config>).

Examples:
    Upload dir to aws bucket:

        python rti_aws_uploader.py -b bucket_name -d /home/user/directory

    List items from aws bucket with custom credentials file location:

        python rti_aws_uploader.py -b bucket_name -c ./folder/config -l

    Delete folder in aws bucket:

        python rti_aws_uploader.py -b bucket_name -r test_directory_name
"""

import os
import sys
import boto3
import botocore
import argparse

from pathlib import Path
from typing import List, Set


def bucket_exists(s3, bucket_name: str):
    """Checks if bucket exists.

    Args:
        s3 (boto3.resource('s3')): s3 resource to check the bucket from.
        bucket_name (str): Selected bucket name to check.

    Returns:
        exists (bool): True if the bucket exists.
        error (str): Error message
    """
    exists: bool = True
    error: str = ''
    try:
        s3.meta.client.head_bucket(Bucket=bucket_name)
    except botocore.client.ClientError as e:
        if e.response['Error']['Code'] == '404':
            exists = False
            error = f'Bucket {bucket_name} not found.'
    except botocore.exceptions.NoCredentialsError:
        exists = False
        error = 'Credentials not found.'
    return exists, error


def list_bucket_objects(s3, bucket_name: str) -> None:
    """Lists objects inside a bucket.

    Args:
        s3 (boto3.resource('s3')): s3 resource.
        bucket_name (str): bucket name from which objects will be listed.
    """
    bucket = s3.Bucket(bucket_name)
    obj_list = [s3.Object(bucket_name, obj_summary.key)
                for obj_summary in bucket.objects.all()]
    if obj_list:
        print('Listing bucket Objects:')
        for obj in obj_list:
            print(f'Obj ({obj.last_modified}) {obj.key}')
    else:
        print('The bucket is empty.')


def _get_directory_tree_file_path_list(
        top_dir: Path, filtered_out_subdirs: Set[str] = None) -> List[Path]:
    """Obtains directory tree structure.

    Args:
        top_dir (Path): top directory path.
        filtered_out_subdirs (Set[str]): filtered out subdir list
            (default=None).

    Returns:
        file_list (List[Path]): all file paths of the tree.
    """
    file_list: List[Path] = []
    for curr_path, _, files in os.walk(top_dir):
        dir_path = Path(curr_path).resolve()
        if (filtered_out_subdirs and
                filtered_out_subdirs.intersection(dir_path.parts)):
            continue
        for curr_file in files:
            file_list.append(dir_path.joinpath(curr_file).resolve())
    return file_list


def _get_filtered_file_list(
        top_dir: Path,
        selected_top_items: Set[str] = None,
        filtered_out_subdirs: Set[str] = None) -> List[Path]:
    """Obtains a list of files under top_dir with their own paths.

    Args:
        top_dir (Path): top directory path.
        selected_top_items (Set[Path]): selected top items list (default=None).
        filtered_out_subdirs (Set[Path]): filtered out subdir list
            (default=None).

    Returns:
        Sorted file path list.
    """
    file_list: List[Path] = []
    for child in top_dir.iterdir():
        if selected_top_items and child.name not in selected_top_items:
            continue
        if child.is_dir():
            file_list.extend(
                _get_directory_tree_file_path_list(child.resolve(),
                                                   filtered_out_subdirs))
        else:
            file_list.append(child.resolve())
    return sorted(file_list)


def upload_directory(s3,
                     bucket_name: str,
                     local_dir_path: Path,
                     selected_top_items: Set[str],
                     filtered_out_subdirs: Set[str]) -> None:
    """Uploads local directory tree to the bucket.

    Args:
        s3 (boto3.resource('s3')): s3 resource.
        bucket_name (str): bucket name.
        local_dir_path (Path): directory path to upload.
        selected_top_items (Set[Path]): selected top items list (default=None).
        filtered_out_subdirs (Set[Path]): filtered out subdir list
            (default=None).
    """
    bucket = s3.Bucket(bucket_name)
    file_path_filtered_list = _get_filtered_file_list(
        local_dir_path, selected_top_items, filtered_out_subdirs)
    print('The following files will be uploaded:')
    print(*file_path_filtered_list, sep='\n')
    if input('Are you sure? (yes/no): ').strip() != 'yes':
        return
    for file_path in file_path_filtered_list:
        source = file_path
        target = file_path.relative_to(local_dir_path.parent)
        print('',
              f'source: {str(source)}',
              f'target: {str(target)}', sep='\n')
        bucket.upload_file(Filename=str(source), Key=str(target))


def remove_directory(s3,
                     bucket_name: str,
                     remote_dir_name: str) -> None:
    """Deletes directory in bucket.

    Args:
        s3 (boto3.resource('s3')): s3 resource.
        bucket_name (str): bucket name.
        remote_dir_name (str): remote directory name to delete.
    """
    bucket = s3.Bucket(bucket_name)
    file_key_list = [{'Key': obj_summary.key}
                     for obj_summary in bucket.objects.all()
                     if obj_summary.key.startswith(remote_dir_name)]
    if file_key_list:
        print('The following files will be removed:')
        print(*file_key_list, sep='\n')
        if input('Are you sure? (yes/no): ').strip() != 'yes':
            return
        bucket.delete_objects(Delete={'Objects': file_key_list})
    else:
        print(f'Error: Folder "{remote_dir_name}" not found.')


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        allow_abbrev=False,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-b',  type=str,
                        required=True,
                        metavar='bucket_name',
                        dest='bucket_name',
                        help='sets bucket name')
    parser.add_argument('-c',  type=Path,
                        metavar='cfg_path',
                        dest='cfg_path',
                        help='sets custom config file path')
    mutual = parser.add_mutually_exclusive_group(required=True)
    mutual.add_argument('-d',  type=Path,
                        metavar='directory_path',
                        dest='local_dir',
                        help='uploads local directory')
    parser.add_argument('-t',  nargs='+',
                        metavar='item',
                        dest='selected_top_items',
                        help='selects top dirs and top files')
    parser.add_argument('-f',  nargs='+',
                        metavar='item',
                        dest='filtered_out_subdirs',
                        help='filters out every path which contains any of '
                             'this items')
    mutual.add_argument('-r',  type=str,
                        metavar='remove_dir',
                        dest='remote_dir_name',
                        help='removes remote directory')
    mutual.add_argument('-l',  action='store_true',
                        dest='list_bucket_objects',
                        help='list objects in the bucket')
    args = parser.parse_args()

    if args.cfg_path:
        cfg_path = args.cfg_path.resolve()
        if not cfg_path.is_file():
            sys.exit(f'Error: Path "{cfg_path}" does not exist.')
        os.environ['AWS_SHARED_CREDENTIALS_FILE'] = str(cfg_path)

    s3 = boto3.resource('s3')

    exists, error = bucket_exists(s3, args.bucket_name)
    if not exists:
        sys.exit(f'Error: {error}')

    if args.list_bucket_objects:
        list_bucket_objects(s3, args.bucket_name)
    elif args.local_dir:
        local_dir = args.local_dir.resolve()

        if not local_dir.exists():
            sys.exit(f'Error: Path "{local_dir}" does not exist.')

        selected_top_items: Set[str] = None
        if args.selected_top_items:
            selected_top_items = set(args.selected_top_items)

        filtered_out_subdirs: Set[str] = None
        if args.filtered_out_subdirs:
            filtered_out_subdirs = set(args.filtered_out_subdirs)

        upload_directory(s3, args.bucket_name, local_dir,
                         selected_top_items,
                         filtered_out_subdirs)
    else:
        remove_directory(s3, args.bucket_name, args.remote_dir_name)


if __name__ == '__main__':
    main()
