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

def publishInProgressCheck(Map config) {
    publishChecks(
        name: config.get('name', STAGE_NAME),
        title: config.title,
        summary: config.summary,
        status: 'IN_PROGRESS',
        text: readFile(env.RTI_JENKINS_OUTPUT_FILE),
        detailsURL: DETAILS_URL,
    )
}

def publishPassedCheck(Map config) {
    publishChecks(
        name: config.get('name', STAGE_NAME),
        title: 'Passed',
        summary: config.summary,
        status: 'COMPLETED',
        conclusion: 'SUCCESS',
        text: readFile(env.RTI_JENKINS_OUTPUT_FILE),
        detailsURL: DETAILS_URL,
    )
}

def publishFailedCheck(Map config) {
    publishChecks(
        name: config.get('name', STAGE_NAME),
        title: 'Failed',
        summary: config.summary,
        status: 'COMPLETED',
        conclusion: 'FAILURE',
        text: readFile(env.RTI_JENKINS_OUTPUT_FILE),
        detailsURL: DETAILS_URL,
    )
}

def publishAbortedCheck(Map config) {
    publishChecks(
        name: config.get('name', STAGE_NAME),
        title: 'Aborted',
        summary: config.summary,
        status: 'COMPLETED',
        conclusion: 'CANCELED',
        text: readFile(env.RTI_JENKINS_OUTPUT_FILE),
        detailsURL: DETAILS_URL,
    )
}

def buildStage(config) {
    def stage = {
        def stageName = "Build ${config.buildMode}/${config.linkMode}"
        stage(stageName) {
            env.RTI_LOGS_FILE = "${env.WORKSPACE}/output_${config.buildMode}_${config.linkMode}.log"
            env.RTI_JENKINS_OUTPUT_FILE = "${env.WORKSPACE}/jenkins_output_${config.buildMode}_${config.linkMode}.md"
            publishInProgressCheck(
                title: 'Building',
                summary: ':wrench: Building all the examples...',
            )
            cmd = "python3 ${env.WORKSPACE}/resources/ci_cd/linux_build.py"
            cmd += " --build-mode ${config.buildMode}"
            cmd += " --link-mode ${config.linkMode}"
            cmd += " --build-dir build_${config.buildMode}_${config.linkMode}"
            cmd += ' | tee $RTI_LOGS_FILE'
            def returnCode = sh(
                """#!/bin/bash
                    set -o pipefail
                    ${cmd}
                """,
                returnStatus=true,
            )

            sh 'python3 resources/ci_cd/jenkins_output.py'
            if (returnCode) {
                publishFailedCheck(
                    summary: ':warning: There was an error building the examples.'
                )
                error(
                    'There were errors building the examples in'
                    + " ${config.buildMode} mode when linking in"
                    + " ${config.linkMode} mode."
                )
            }
            publishPassedCheck(
                summary: ':white_check_mark: All the examples were built succesfully.'
            )
        }
    }
    return stage
}

def buildAllStages() {
    def buildTypes = ["release", "debug"]
    def linkTypes = ["dynamic", "static"]
    def stages = [:]

    for (buildMode in buildTypes) {
        for (linkMode in linkTypes) {
            stages["${buildMode}/${linkMode}"] = buildStage(
                buildMode: buildMode,
                linkMode: linkMode,
            )
        }
    }

    return stages
}

pipeline {
    agent none

    stages {
        stage('Executor Check') {
            steps {
                publishInProgressCheck(
                    name: 'Waiting for executor',
                    title: 'Waiting',
                    summary: ':hourglass: Waiting for next available executor...'
                )
            }
        }

        stage('Build sequence') {
            agent {
                dockerfile {
                    filename 'resources/docker/Dockerfile.x64Linux4gcc7.3.0'
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
                        publishPassedCheck(
                            name: 'Waiting for executor',
                            title: 'Passed',
                            summary: ':white_check_mark: Executor found.',
                        )

                        script {
                            connextdds_arch = sh(
                                script: 'echo $CONNEXTDDS_ARCH',
                                returnStdout: true
                            ).trim()
                        }

                        sh 'python3 resources/ci_cd/jenkins_output.py'
                        publishInProgressCheck(
                            title: 'Downloading',
                            summary: ':arrow_down: Downloading RTI Connext DDS libraries...',
                        )

                        rtDownload (
                            serverId: 'rti-artifactory',
                            spec: """{
                                "files": [
                                {
                                    "pattern": "connext-ci/pro/weekly/",
                                    "props": "rti.artifact.architecture=${connextdds_arch};rti.artifact.kind=staging",
                                    "sortBy": ["created"],
                                    "sortOrder": "desc",
                                    "limit": 1,
                                    "flat": true
                                }]
                            }""",
                        )

                        // We cannot use the explode option because it is bugged.
                        // https://www.jfrog.com/jira/browse/HAP-1154
                        sh 'tar zxvf connextdds-staging-${CONNEXTDDS_ARCH}.tgz unlicensed/'

                        sh '''
                            cp ${RTI_INSTALLATION_PATH}/rti_connext_dds-*/lib/${CONNEXTDDS_ARCH}/openssl-1.*/* \
                                ${RTI_INSTALLATION_PATH}/rti_connext_dds-*/lib/${CONNEXTDDS_ARCH}/
                        '''
                    }

                    post {
                        always {
                            sh 'python3 resources/ci_cd/jenkins_output.py'
                        }
                        success {
                            publishPassedCheck(
                                title: 'Passed',
                                summary: ':white_check_mark: RTI Connext DDS libraries downloaded.',
                            )
                        }
                        failure {
                            publishFailedCheck(
                                title: 'Failed',
                                summary: ':warning: Failed downloading RTI Connext DDS libraries.',
                            )
                        }
                        aborted {
                            publishAbortedCheck(
                                title: 'Aborted',
                                summary: ':no_entry: The download of RTI Connext DDS libraries was aborted.',
                            )
                        }
                    }
                }

                stage('Build') {
                    parallel buildAllStages()
                }

                stage('Static Analysis') {
                    steps {
                        publishPassedCheck(
                            title: 'In progress',
                            summary: ':mag: Analysing all the examples...',
                        )

                        sh '''#!/bin/bash
                            set -o pipefail
                            python3 resources/ci_cd/linux_static_analysis.py | tee ${RTI_LOGS_FILE}
                        '''
                    }

                    post {
                        always {
                            sh 'python3 resources/ci_cd/jenkins_output.py'
                        }
                        success {
                            publishPassedCheck(
                                title: 'Passed',
                                summary: ':white_check_mark: Succesfully analysed',
                            )
                        }
                        failure {
                            publishFailedCheck(
                                title: 'Failed',
                                summary: ':warning: The static analysis failed',
                            )
                        }
                        aborted {
                            publishAbortedCheck(
                                title: 'Aborted',
                                summary: ':no_entry: The static analysis was aborted',
                            )
                        }
                    }
                }
            }

            post {
                cleanup {
                    cleanWs()
                }
                aborted {
                    publishAbortedCheck(
                        name: 'Waiting for executor',
                        title: 'Aborted',
                        summary: ':no_entry: The pipeline was aborted',
                    )
                }
            }
        }
    }
}

