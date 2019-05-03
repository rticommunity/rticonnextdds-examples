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
import argparse

from pathlib import Path
from typing import List, Set, Dict
from boto3_type_annotations import s3
from botocore.client import ClientError
from botocore.exceptions import NoCredentialsError


def bucket_exists(s3_resource: s3.ServiceResource, bucket_name: str) -> bool:
    """Checks if bucket exists.

    Args:
        s3_resource: s3 resource to check the bucket from.
        bucket_name: Selected bucket name to check.

    Returns:
        exists: True if the bucket exists.

    Raises:
        NoCredentialsError: If credentials are not found.
    """
    exists: bool = True
    try:
        s3_resource.meta.client.head_bucket(Bucket=bucket_name)
    except ClientError as e:
        if e.response["Error"]["Code"] == "404":
            exists = False
    return exists


def list_bucket_objects(
    s3_resource: s3.ServiceResource, bucket_name: str
) -> None:
    """Lists objects inside a bucket.

    Args:
        s3_resource: s3 resource.
        bucket_name: bucket name from which objects will be listed.
    """
    bucket: s3.Bucket = s3_resource.Bucket(bucket_name)
    obj_list: List[s3.Object] = [
        s3_resource.Object(bucket_name, obj_summary.key)
        for obj_summary in bucket.objects.all()
    ]
    if obj_list:
        print("Listing bucket Objects:")
        for obj in obj_list:
            print(f"Obj ({obj.last_modified}) {obj.key}")
    else:
        print("The bucket is empty.")


def _get_directory_tree_file_path_list(
    top_dir: Path, filtered_out_items: Set[str] = None
) -> List[Path]:
    """Obtains directory tree structure.

    Args:
        top_dir: top directory path.
        filtered_out_items: filtered out subdir list (default=None).

    Returns:
        file_list: all file paths of the tree.
    """
    file_list: List[Path] = []
    for curr_path, _, files in os.walk(top_dir):
        dir_path = Path(curr_path).resolve()
        if filtered_out_items and filtered_out_items.intersection(
            dir_path.parts
        ):
            continue
        for curr_file in files:
            file_list.append(dir_path.joinpath(curr_file).resolve())
    return file_list


def _get_filtered_file_list(
    top_dir: Path,
    selected_top_items: Set[str] = None,
    filtered_out_items: Set[str] = None,
) -> List[Path]:
    """Obtains a list of files under top_dir with their own paths.

    Args:
        top_dir: top directory path.
        selected_top_items: selected top items list (default=None).
        filtered_out_items: filtered out subdir list (default=None).

    Returns:
        Sorted file path list.
    """
    file_list: List[Path] = []
    for child in top_dir.iterdir():
        if selected_top_items and child.name not in selected_top_items:
            continue
        if child.is_dir():
            file_list.extend(
                _get_directory_tree_file_path_list(
                    child.resolve(), filtered_out_items
                )
            )
        else:
            file_list.append(child.resolve())
    return sorted(file_list)


def upload_directory(
    s3_resource: s3.ServiceResource,
    bucket_name: str,
    local_dir_path: Path,
    selected_top_items: Set[str],
    filtered_out_items: Set[str],
) -> None:
    """Uploads local directory tree to the bucket.

    Args:
        s3_resource: s3 resource.
        bucket_name: bucket name.
        local_dir_path: directory path to upload.
        selected_top_items: selected top items list (default=None).
        filtered_out_items: filtered out subdir list (default=None).
    """
    bucket: s3.Bucket = s3_resource.Bucket(bucket_name)
    file_path_filtered_list = _get_filtered_file_list(
        local_dir_path, selected_top_items, filtered_out_items
    )
    print("The following files will be uploaded:")
    print(*file_path_filtered_list, sep="\n")
    if input("Are you sure? (yes/no): ").strip() != "yes":
        return
    for file_path in file_path_filtered_list:
        source: Path = file_path
        target: Path = file_path.relative_to(local_dir_path.parent)
        print("", f"source: {str(source)}", f"target: {str(target)}", sep="\n")
        bucket.upload_file(Filename=str(source), Key=str(target))


def remove_directory(
    s3_resource: s3.ServiceResource, bucket_name: str, remote_dir_name: str
) -> None:
    """Deletes directory in bucket.

    Args:
        s3_resource: s3 resource.
        bucket_name: bucket name.
        remote_dir_name: remote directory name to delete.
    """
    bucket: s3.Bucket = s3_resource.Bucket(bucket_name)
    file_key_list: List[Dict[str, str]] = [
        {"Key": obj_summary.key}
        for obj_summary in bucket.objects.all()
        if obj_summary.key.startswith(remote_dir_name)
    ]
    if file_key_list:
        print("The following files will be removed:")
        print(*file_key_list, sep="\n")
        if input("Are you sure? (yes/no): ").strip() != "yes":
            return
        bucket.delete_objects(Delete={"Objects": file_key_list})
    else:
        print(f'Error: Folder "{remote_dir_name}" not found.')


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        allow_abbrev=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "-b",
        "--bucket",
        type=str,
        required=True,
        metavar="name",
        dest="bucket_name",
        help="sets bucket name",
    )
    parser.add_argument(
        "-c",
        "--config",
        type=Path,
        metavar="path",
        dest="config_path",
        help="sets custom config file path",
    )
    mutual = parser.add_mutually_exclusive_group(required=True)
    mutual.add_argument(
        "-u",
        "--upload-directory",
        type=Path,
        metavar="path",
        dest="local_dir",
        help="uploads local directory",
    )
    parser.add_argument(
        "-t",
        "--selected-top-items",
        nargs="+",
        metavar="item",
        dest="selected_top_items",
        help="selects top dirs and files",
    )
    parser.add_argument(
        "-f",
        "--filtered-out-items",
        nargs="+",
        metavar="item",
        dest="filtered_out_items",
        help="filters out every path which contains any of this items",
    )
    mutual.add_argument(
        "-r",
        "--remove-directory",
        type=str,
        metavar="name",
        dest="remote_dir_name",
        help="removes remote directory",
    )
    mutual.add_argument(
        "-l",
        "--list-objects",
        action="store_true",
        dest="list_bucket_objects",
        help="list objects in the bucket",
    )
    args = parser.parse_args()

    if args.config_path:
        config_path: Path = args.config_path.resolve()
        if not config_path.is_file():
            sys.exit(f'Error: Path "{config_path}" does not exist.')
        os.environ["AWS_SHARED_CREDENTIALS_FILE"] = str(config_path)

    s3_resource: s3.ServiceResource = boto3.resource("s3")

    try:
        if not bucket_exists(s3_resource, args.bucket_name):
            sys.exit(f'Error: Bucket "{args.bucket_name}" does not exist.')
    except NoCredentialsError:
        sys.exit("Error: Credentials not found.")

    if args.list_bucket_objects:
        list_bucket_objects(s3_resource, args.bucket_name)
    elif args.local_dir:
        local_dir: Path = args.local_dir.resolve()

        if not local_dir.exists():
            sys.exit(f'Error: Path "{local_dir}" does not exist.')

        selected_top_items: Set[str] = None
        if args.selected_top_items:
            selected_top_items = set(args.selected_top_items)

        filtered_out_items: Set[str] = None
        if args.filtered_out_items:
            filtered_out_items = set(args.filtered_out_items)

        upload_directory(
            s3_resource,
            args.bucket_name,
            local_dir,
            selected_top_items,
            filtered_out_items,
        )
    else:
        remove_directory(s3_resource, args.bucket_name, args.remote_dir_name)


if __name__ == "__main__":
    main()
