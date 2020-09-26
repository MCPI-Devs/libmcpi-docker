pipeline {
    agent {
        dockerfile {
            filename 'Dockerfile.build'
            args '-v /var/run/docker.sock:/var/run/docker.sock'
        }
    }
    stages {
        stage('Install QEMU') {
            steps {
                sh 'docker run --rm --privileged multiarch/qemu-user-static --reset -p yes'
            }
        }
        stage('Build') {
            steps {
                sh './build.sh'
            }
        }
        stage('Publish') {
            steps {
                withCredentials([usernamePassword(credentialsId: 'docker_hub_login', usernameVariable: 'DOCKER_HUB_USERNAME', passwordVariable: 'DOCKER_HUB_PASSWORD')]) {
                    sh 'docker login -u "${DOCKER_HUB_USERNAME}" -p "${DOCKER_HUB_PASSWORD}"'
                }
                sh 'docker push thebrokenrail/minecraft-pi'
            }
        }
    }
}
