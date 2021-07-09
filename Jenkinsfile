/*
 * (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */
def DETAILS_URL="https://community.rti.com/"
def detailsText

pipeline {
    agent none

    stages {
        stage('Executor Check') {
            steps {
                publishChecks detailsURL: DETAILS_URL, name: 'Waiting for executor',
                        status: 'IN_PROGRESS', title: 'Waiting',
                        summary: ':hourglass: Waiting for next available executor...'
            }
        }

        stage('Build sequence') {
            agent {
                dockerfile {
                    filename 'Dockerfile'
                    label 'docker'
                }
            }

            environment {
                RTI_INSTALLATION_PATH = "${WORKSPACE}/unlicensed"
                RTI_LOGS_FILE = "${WORKSPACE}/output_logs.txt"
            }

            stages {
                stage('Download Packages') {
                    steps {
                        publishChecks detailsURL: DETAILS_URL, name: 'Waiting for executor',
                                summary: ':white_check_mark: Build started.',
                                title: 'Passed'

                        sh 'python3 resources/ci_cd/jenkins_output.py'

                        script {
                            detailsText = readFile("jenkins_output.md")
                        }

                        publishChecks detailsURL: DETAILS_URL, name: STAGE_NAME,
                            status: 'IN_PROGRESS', title: 'Downloading', text: detailsText,
                            summary: ':arrow_down: Downloading RTI Connext DDS libraries...'

                        rtDownload (
                            serverId: 'rti-artifactory',
                            spec: '''{
                                "files": [
                                {
                                    "pattern": "connext-ci/pro/weekly/",
                                    "props": "rti.artifact.architecture=x64Linux4gcc7.3.0;rti.artifact.kind=staging",
                                    "sortBy": ["created"],
                                    "sortOrder": "desc",
                                    "limit": 1,
                                    "flat": true
                                }]
                            }''',
                        )

                        // We cannot use the explode option because it is bugged.
                        // https://www.jfrog.com/jira/browse/HAP-1154
                        sh 'tar zxvf connextdds-staging-x64Linux4gcc7.3.0.tgz unlicensed/'
                        
                        sh 'python3 resources/ci_cd/jenkins_output.py'

                        script {
                            detailsText = readFile("jenkins_output.md")
                        }
                    }

                    post {
                        success {
                            publishChecks detailsURL: DETAILS_URL, name: STAGE_NAME,
                                summary: ':white_check_mark: RTI Connext DDS libraries downloaded.',
                                title: 'Passed', text: detailsText
                        }
                        failure {
                            publishChecks conclusion: 'FAILURE', detailsURL: DETAILS_URL,
                                name: STAGE_NAME, title: 'Failed', text: detailsText,
                                summary: ':warning: Failed downloading RTI Connext DDS libraries.'
                        }
                        aborted {
                            publishChecks conclusion: 'CANCELED', detailsURL: DETAILS_URL,
                                name: STAGE_NAME, title: 'Aborted', text: detailsText,
                                summary: ':no_entry: The download of RTI Connext DDS libraries was aborted.'
                        }
                    }
                }

                stage('Build') {
                    steps {
                        publishChecks detailsURL: DETAILS_URL, name: STAGE_NAME, 
                            status: 'IN_PROGRESS', summary: ':wrench: Building all the examples...', 
                            title: 'Building', text: detailsText

                        sh  """#!/bin/bash
                               set -o pipefail
                               python3 resources/ci_cd/linux_build.py | tee $RTI_LOGS_FILE
                            """
                    }

                    post {
                        always{
                            sh 'python3 resources/ci_cd/jenkins_output.py'
                        }
                        success {
                            publishChecks detailsURL: DETAILS_URL, name: STAGE_NAME,
                                summary: ':white_check_mark: All the examples were built succesfully.',
                                title: 'Passed', text: readFile("jenkins_output.md")
                        }
                        failure {
                            publishChecks conclusion: 'FAILURE', detailsURL: DETAILS_URL,
                                name: STAGE_NAME, title: 'Failed', text: readFile("jenkins_output.md"),
                                summary: ':warning: There was an error building the examples.'
                        }
                        aborted {
                            publishChecks conclusion: 'CANCELED', detailsURL: DETAILS_URL,
                                name: STAGE_NAME, title: 'Aborted', text: readFile("jenkins_output.md"),
                                summary: ':no_entry: The examples build was aborted'
                        }
                    }
                }

                stage('Static Analysis') {
                    steps {
                        publishChecks detailsURL: DETAILS_URL, name: STAGE_NAME, 
                            status: 'IN_PROGRESS', title: 'In progress', text: detailsText,
                            summary: ':mag: Analysing all the examples...'

                        sh  """#!/bin/bash
                               set -o pipefail
                               python3 resources/ci_cd/linux_static_analysis.py | tee $RTI_LOGS_FILE
                            """
                    }

                    post {
                        always {
                            sh 'python3 resources/ci_cd/jenkins_output.py'
                        }
                        success {
                            publishChecks detailsURL: DETAILS_URL, name: STAGE_NAME,
                                summary: ':white_check_mark: Succesfully analysed',
                                title: 'Passed', text: readFile("jenkins_output.md")
                        }
                        failure {
                            publishChecks conclusion: 'FAILURE', detailsURL: DETAILS_URL,
                                name: STAGE_NAME, title: 'Failed', text: readFile("jenkins_output.md"),
                                summary: ':warning: The static analysis failed'
                        }
                        aborted {
                            publishChecks conclusion: 'CANCELED', detailsURL: DETAILS_URL,
                                name: STAGE_NAME, title: 'Aborted', text: readFile("jenkins_output.md"),
                                summary: ':no_entry: The static analysis was aborted'
                        }
                    }
                }
            }

            post {
                cleanup {
                    cleanWs()
                }
                aborted {
                    publishChecks conclusion: 'CANCELED', detailsURL: DETAILS_URL,
                        name: 'Waiting for executor', title: 'Aborted',
                       summary: ':no_entry: The pipeline was aborted'
                }
            }
        }
    }
}

