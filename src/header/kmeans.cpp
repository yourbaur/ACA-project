#include <iostream>
#include <vector>
#include <cstdlib>
#include <xlsxio_read.h>

// Define a structure to represent a customer with features
struct Customer {
    double firstPurchaseDate;
    double lastPurchaseDate;
    double totalOrders;
    double totalQuantity;
    double averagePurchasePrice;
};

// Function to calculate Euclidean distance between two customers
double distance(const Customer& c1, const Customer& c2) {
    // Calculate Euclidean distance using relevant features
    double dist = 0.0;
    dist += pow(c1.firstPurchaseDate - c2.firstPurchaseDate, 2);
    dist += pow(c1.lastPurchaseDate - c2.lastPurchaseDate, 2);
    dist += pow(c1.totalOrders - c2.totalOrders, 2);
    dist += pow(c1.totalQuantity - c2.totalQuantity, 2);
    dist += pow(c1.averagePurchasePrice - c2.averagePurchasePrice, 2);
    return sqrt(dist);
}

// Function to assign customers to the nearest centroid
void assignClusters(const std::vector<Customer>& customers, const std::vector<Customer>& centroids, std::vector<int>& assignments) {
    for (int i = 0; i < customers.size(); ++i) {
        double minDist = std::numeric_limits<double>::max();
        int closestCentroid = -1;
        for (int j = 0; j < centroids.size(); ++j) {
            double dist = distance(customers[i], centroids[j]);
            if (dist < minDist) {
                minDist = dist;
                closestCentroid = j;
            }
        }
        assignments[i] = closestCentroid;
    }
}

int main() {
    // Open Excel file
    const char* filename = "customer_data.xlsx";
    xlsxioreader xls_reader = xlsxioread_open(filename);
    if (!xls_reader) {
        std::cerr << "Error opening Excel file" << std::endl;
        return 1;
    }

    // Read customer data from Excel file
    std::vector<Customer> customers;
    XLSXIOCHAR* cell_data;
    while ((cell_data = xlsxioread_next_cell(xls_reader)) != NULL) {
        // Read each cell data and construct Customer object
        Customer customer;
        customer.firstPurchaseDate = atof(cell_data);
        cell_data = xlsxioread_next_cell(xls_reader);
        customer.lastPurchaseDate = atof(cell_data);
        cell_data = xlsxioread_next_cell(xls_reader);
        customer.totalOrders = atof(cell_data);
        cell_data = xlsxioread_next_cell(xls_reader);
        customer.totalQuantity = atof(cell_data);
        cell_data = xlsxioread_next_cell(xls_reader);
        customer.averagePurchasePrice = atof(cell_data);
        customers.push_back(customer);
        xlsxioread_free(cell_data);
    }
    xlsxioread_close(xls_reader);

    // Number of clusters
    int k = 3;

    // Initialize centroids randomly
    std::vector<Customer> centroids;
    for (int i = 0; i < k; ++i) {
        centroids.push_back(customers[std::rand() % customers.size()]);
    }

    // Assign customers to clusters
    std::vector<int> assignments(customers.size());
    assignClusters(customers, centroids, assignments);

    // Output cluster assignments
    for (int i = 0; i < assignments.size(); ++i) {
        std::cout << "Customer " << i << " belongs to cluster " << assignments[i] << std::endl;
    }

    return 0;
}

