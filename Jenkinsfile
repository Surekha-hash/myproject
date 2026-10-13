
pipeline {
    agent any

    stages {

        stage('Checkout') {
            steps {
                echo 'Checking out source code from GitHub...'
                checkout scm
            }
        }

        stage('Validate Docker Compose') {
            steps {
                echo 'Validating Docker Compose configuration...'
                sh 'docker compose config -q'
            }
        }

        stage('Build Docker Images') {
            steps {
                echo 'Building Docker images...'
                sh 'docker compose build'
            }
        }

        stage('Verify Docker Images') {
            steps {
                echo 'Verifying that all required images exist...'

                sh '''
                    set -e

                    docker image inspect myproject-network:latest
                    docker image inspect myproject-billing:latest
                    docker image inspect myproject-control:latest

                    echo "All required Docker images are available."
                '''
            }
        }

        stage('Trivy Vulnerability Scan') {
            steps {
                script {
                    def images = [
                        'myproject-network:latest',
                        'myproject-billing:latest',
                        'myproject-control:latest'
                    ]

                    for (image in images) {
                        echo "Scanning image: ${image}"

                        int scanStatus = sh(
                            script: """
                                trivy image \
                                  --severity HIGH,CRITICAL \
                                  --exit-code 0 \
                                  ${image}
                            """,
                            returnStatus: true
                        )

                        if (scanStatus != 0) {
                            echo "WARNING: Trivy scan encountered an error for ${image}. Continuing pipeline."
                        } else {
                            echo "Trivy scan completed for ${image}. Review the report above for vulnerabilities."
                        }
                    }
                }
            }
        }

        stage('Network Tests') {
            steps {
                echo 'Starting network service for testing...'

                sh '''
                    set -e

                    docker rm -f network-test-container 2>/dev/null || true

                    docker run -d \
                      --name network-test-container \
                      -p 127.0.0.1:19001:9001 \
                      myproject-network:latest

                    echo "Waiting for network service to become ready..."

                    for i in $(seq 1 30); do
                        if python3 -c "
import socket
s = socket.create_connection(('127.0.0.1', 19001), timeout=1)
s.close()
" 2>/dev/null; then
                            echo "Network service is ready."
                            break
                        fi

                        if [ "$i" -eq 30 ]; then
                            echo "ERROR: Network service did not become ready."
                            docker logs network-test-container
                            exit 1
                        fi

                        sleep 1
                    done

                    echo "Running network integration tests..."
                    python3 tests/test_network.py 19001
                '''
            }
        }
    }

    post {
        always {
            echo 'Cleaning up test containers...'

            sh '''
                docker rm -f network-test-container 2>/dev/null || true
            '''
        }

        success {
            echo 'Pipeline completed successfully!'
        }

        failure {
            echo 'Pipeline failed. Check the console output for details.'
        }
    }
}
