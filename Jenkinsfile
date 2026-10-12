
pipeline {
    agent any

    options {
        timestamps()
    }

    environment {
        COMPOSE_PROJECT_NAME = 'myproject'
        GIT_REPO = 'https://github.com/Surekha-hash/myproject.git'
        GIT_BRANCH = 'main'
        TEST_CONTAINER = 'myproject-network-test'
        TEST_PORT = '19001'
    }

    stages {
        stage('Checkout') {
            steps {
                git branch: "${GIT_BRANCH}",
                    url: "${GIT_REPO}"
            }
        }

        stage('Validate Docker Compose') {
            steps {
                sh 'docker compose config -q'
                echo 'Docker Compose configuration is valid.'
            }
        }

        stage('Build Docker Images') {
            steps {
                sh 'docker compose build'
                echo 'Docker images built successfully.'
            }
        }

        stage('Verify Images') {
            steps {
                sh '''
                    docker image inspect myproject-network:latest
                    docker image inspect myproject-billing:latest
                    docker image inspect myproject-control:latest
                '''
                echo 'All three Docker images verified successfully.'
            }
        }

        stage('Test Network Service') {
            steps {
                sh '''
                    set -eu

                    docker rm -f "$TEST_CONTAINER" >/dev/null 2>&1 || true

                    docker run -d \
                        --name "$TEST_CONTAINER" \
                        -p "127.0.0.1:${TEST_PORT}:9001" \
                        myproject-network:latest \
                        /app/vse-network

                    # Wait for the TCP service to start.
                    ready=0
                    for i in $(seq 1 15); do
                        if python3 -c "
import socket
s = socket.socket()
s.settimeout(1)
try:
    s.connect(('127.0.0.1', ${TEST_PORT}))
    s.close()
except OSError:
    raise SystemExit(1)
" 2>/dev/null; then
                            ready=1
                            break
                        fi
                        sleep 1
                    done

                    if [ "$ready" -ne 1 ]; then
                        echo "Network service did not start."
                        docker logs "$TEST_CONTAINER"
                        exit 1
                    fi

                    python3 tests/test_network.py "$TEST_PORT"
                '''
            }
        }
    }

    post {
        always {
            sh '''
                docker rm -f myproject-network-test >/dev/null 2>&1 || true
            '''
        }

        success {
            echo 'CI pipeline and network tests completed successfully.'
        }

        failure {
            echo 'Pipeline or tests failed. Check the console output.'
        }
    }
}
