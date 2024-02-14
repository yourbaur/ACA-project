#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <ctime>
#include "/usr/local/opt/libomp/include/omp.h" // Include OpenMP header

using namespace std;

// Structure to represent a customer
struct Point {
    double age, income, spendingScore, frequency;
    int cluster; // Cluster to which the customer belongs
    // Constructor
    Point(double _age, double _income, double _spendingScore, double _frequency, int _cluster = -1)
        : age(_age), income(_income), spendingScore(_spendingScore), frequency(_frequency), cluster(_cluster) {}
};

// Function to calculate Euclidean distance between two customers
double euclideanDistance(Point p1, Point p2) {
    double distance = 0.0;
    #pragma omp parallel for reduction(+:distance) // OpenMP parallelization directive
    for (int i = 0; i < 4; ++i) {
        double diff = 0.0;
        switch (i) {
            case 0: diff = p1.age - p2.age; break;
            case 1: diff = p1.income - p2.income; break;
            case 2: diff = p1.spendingScore - p2.spendingScore; break;
            case 3: diff = p1.frequency - p2.frequency; break;
        }
        distance += diff * diff;
    }
    return sqrt(distance);
}

// Function to update cluster centroids
void updateCentroids(vector<Point>& points, vector<Point>& centroids) {
    for (int i = 0; i < centroids.size(); ++i) {
        double sumAge = 0, sumIncome = 0, sumSpendingScore = 0, sumFrequency = 0;
        int count = 0;
        for (const Point& point : points) {
            if (point.cluster == i) {
                sumAge += point.age;
                sumIncome += point.income;
                sumSpendingScore += point.spendingScore;
                sumFrequency += point.frequency;
                count++;
            }
        }
        if (count > 0) {
            centroids[i].age = sumAge / count;
            centroids[i].income = sumIncome / count;
            centroids[i].spendingScore = sumSpendingScore / count;
            centroids[i].frequency = sumFrequency / count;
        }
    }
}

// Function to check if centroids have converged
bool hasConverged(vector<Point>& oldCentroids, vector<Point>& newCentroids, double epsilon) {
    for (int i = 0; i < oldCentroids.size(); ++i) {
        if (euclideanDistance(oldCentroids[i], newCentroids[i]) > epsilon) {
            return false;
        }
    }
    return true;
}

// K-means clustering function
void kmeans(vector<Point>& points, vector<Point>& centroids, double epsilon) {
    // Assign points to the nearest centroids and update centroids iteratively
    vector<Point> oldCentroids;
    do {
        oldCentroids = centroids;
        for (Point& point : points) {
            double minDistance = numeric_limits<double>::max();
            int closestCluster = -1;
            for (int i = 0; i < centroids.size(); ++i) {
                double distance = euclideanDistance(point, centroids[i]);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestCluster = i;
                }
            }
            point.cluster = closestCluster;
        }
        updateCentroids(points, centroids);
    } while (!hasConverged(oldCentroids, centroids, epsilon));
}

int main() {
    // Read customer data from a file (format: age income spendingScore frequency)
    vector<Point> points;
    ifstream file("MOCK_DATA.txt");
    if (file.is_open()) {
        double age, income, spendingScore, frequency;
        while (file >> age >> income >> spendingScore >> frequency) {
            points.push_back(Point(age, income, spendingScore, frequency, -1)); // -1 cluster indicates unassigned
        }
        file.close();
    } else {
        cerr << "Unable to open file." << endl;
        return 1;
    }

    // Number of clusters
    int k = 3;

    // Convergence threshold
    double epsilon = 0.001;

    // Initialize centroids randomly
    vector<Point> centroids;
    srand(time(NULL));
    for (int i = 0; i < k; ++i) {
        centroids.push_back(points[rand() % points.size()]);
        centroids.back().cluster = i; // Assign cluster number
    }

    // Perform K-means clustering without parallelism and measure execution time
    auto start = chrono::high_resolution_clock::now();
    kmeans(points, centroids, epsilon);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Execution time (serial): " << duration.count() << " seconds" << endl;

    // Output cluster assignments...

    return 0;
}
