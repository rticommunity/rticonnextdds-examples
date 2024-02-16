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

def runBuildStage(String buildMode, String linkMode) {
    def cmd = "python3 ${env.WORKSPACE}/resources/ci_cd/linux_build.py"
    cmd += " --build-mode ${buildMode}"
    cmd += " --link-mode ${linkMode}"
    cmd += " --build-dir ${get_build_directory(buildMode, linkMode)}"
    sh(cmd)
}

def get_build_directory(String buildMode, String linkMode) {
    return "build_${buildMode}_${linkMode}"
}

pipeline {
    agent none

    options {
        disableConcurrentBuilds()
        /*
            To avoid excessive resource usage in server, we limit the number
            of builds to keep in pull requests
        */
        buildDiscarder(
            logRotator(
                artifactDaysToKeepStr: '',
                artifactNumToKeepStr: '',
                daysToKeepStr: '',
                /*
                   For pull requests only keep the last 10 builds, for regular
                   branches keep up to 20 builds.
                */
                numToKeepStr: changeRequest() ? '10' : '20'
            )
        )
        // Set a timeout for the entire pipeline
        timeout(time: 2, unit: 'HOURS')
    }

    stages {
        stage('Build sequence') {
            agent {
                dockerfile {
                    filename 'resources/docker/Dockerfile.linux'
                    label 'docker'
                }
            }

            environment {
                RTI_INSTALLATION_PATH = "${env.WORKSPACE}"
            }

            stages {
                stage('Download Packages') {
                    steps {
                        sh 'pip3 install -r resources/ci_cd/requirements.txt'

                        withAWSCredentials {
                            withCredentials([
                                string(credentialsId: 's3-bucket', variable: 'RTI_AWS_BUCKET'),
                                string(credentialsId: 's3-path', variable: 'RTI_AWS_PATH'),
                            ]) {
                                sh 'python3 resources/ci_cd/linux_install.py -a $CONNEXTDDS_ARCH'
                            }
                        }
                    }
                }

                stage('Build all modes') {
                    matrix {
                        axes {
                            axis {
                                name 'buildMode'
                                values 'release', 'debug'
                            }
                            axis {
                                name 'linkMode'
                                values 'static', 'dynamic'
                            }
                        }
                        stages {
                            stage('Build single mode') {
                                steps {
                                    echo("Build ${buildMode}/${linkMode}")
                                    runBuildStage(buildMode, linkMode)
                                }
                            }
                        }
                    }
                }

                stage('Static Analysis') {
                    steps {
                        sh "python3 resources/ci_cd/linux_static_analysis.py --build-dir ${get_build_directory('release', 'dynamic')}"
                    }
                }
            }

            post {
                cleanup {
                    cleanWs()
                }
            }
        }
    }
}

