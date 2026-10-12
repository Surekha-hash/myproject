
pipeline {
    agent any

    options {
        timestamps()
    }

    environment {
        COMPOSE_PROJECT_NAME = 'myproject'
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
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
            }
        }

        stage('Verify Images') {
            steps {
                sh '''
                    docker image inspect myproject-network:latest
                    docker image inspect myproject-billing:latest
                    docker image inspect myproject-control:latest
                '''
                echo 'All three VSE images were built successfully.'
            }
        }
    }

    post {
        success {
            echo 'CloudVSE CI pipeline completed successfully.'
        }
        failure {
            echo 'Pipeline failed. Check the console output.'
        }
    }
}
