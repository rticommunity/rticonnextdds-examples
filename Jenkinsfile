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

// TODO: Remove when merged
@Library("rticommunity-jenkins-pipelines@feature/enhance-examples-jenkinsfile") _

/**
 * Build the examples in the specified build and link modes.
 *
 * @param buildMode The build mode name.
 * @param linkMode The link mode name.
 */
void runBuildStage(String buildMode, String linkMode) {
    String cmd = "python3 ${env.WORKSPACE}/resources/ci_cd/linux_build.py"
    cmd += " --build-mode ${buildMode}"
    cmd += " --link-mode ${linkMode}"
    cmd += " --build-dir ${getBuildDirectory(buildMode, linkMode)}"
    runCommand(cmd)
}

/**
 * Craft the build directory name.
 *
 * @param buildMode The build mode name.
 * @param linkMode The link mode name.
 */
String getBuildDirectory(String buildMode, String linkMode) {
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

    parameters {
        string(
            name: 'CMAKE_UTILS_REFERENCE',
            description: '''
                rticommunity/rticonnextdds-cmake-utils repository reference to use (Branch or PR).
                 E.g.: PR-123, release/7.3.0, master
            ''',
            defaultValue: '',
            trim: true,
        )
    }

    stages {
        stage('Build sequence') {
            agent {
                label "${nodeManager.labelFromJobName()}"
            }

            environment {
                RTI_INSTALLATION_PATH = "${env.WORKSPACE}"
                VIRTUAL_ENV = "${env.WORKSPACE}/.venv"
            }

            stages {
                stage('Select CMake utils version') {
                    steps {
                        switchBranch(
                            reference: params.CMAKE_UTILS_REFERENCE,
                            repositoryPath: "${env.WORKSPACE}/resources/cmake/rticonnextdds-cmake-utils"
                        )
                    }
                }

                stage('Download Packages') {
                    steps {
                        nodeManager.runInsideExecutor() {
                            runCommand('pip3 install -r resources/ci_cd/requirements.txt')

                            withAWSCredentials {
                                withCredentials([
                                    string(credentialsId: 's3-bucket', variable: 'RTI_AWS_BUCKET'),
                                    string(credentialsId: 's3-path', variable: 'RTI_AWS_PATH'),
                                ]) {
                                    runCommand('python3 resources/ci_cd/linux_install.py -a $CONNEXTDDS_ARCH')
                                }
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
                                    nodeManager.runInsideExecutor() {
                                        echo("Build ${buildMode}/${linkMode}")
                                        runBuildStage(buildMode, linkMode)
                                    }
                                }
                            }
                        }
                    }
                }

                stage('Static Analysis') {
                    steps {
                        nodeManager.runInsideExecutor() {
                            runCommand("""
                                python3 resources/ci_cd/linux_static_analysis.py \
                                --build-dir ${getBuildDirectory('release', 'dynamic')}
                            """)
                        }
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

