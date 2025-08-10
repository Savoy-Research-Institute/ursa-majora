#ifndef NUMERICAL_ALGORITHMS_H
#define NUMERICAL_ALGORITHMS_H

#include <Arduino.h>
#include "../application_data_types/numeric_types.h"

// Algorithm categories
enum class AlgorithmCategory {
    LINEAR_ALGEBRA,
    STATISTICS,
    OPTIMIZATION,
    INTERPOLATION,
    INTEGRATION,
    DIFFERENTIATION,
    FILTERING,
    SIGNAL_PROCESSING
};

// Algorithm precision levels
enum class AlgorithmPrecision {
    LOW,
    MEDIUM,
    HIGH,
    EXTREME
};

// Algorithm performance metrics
struct AlgorithmPerformance {
    uint32_t executionTime;      // Microseconds
    uint32_t memoryUsage;        // Bytes
    uint32_t iterations;         // Number of iterations
    float accuracy;              // Accuracy percentage
    bool converged;              // Whether algorithm converged
    String errorMessage;         // Error description if failed
};

// Matrix structure
struct Matrix {
    uint16_t rows;
    uint16_t columns;
    float* data;
    bool isSquare() const { return rows == columns; }
    bool isValid() const { return data != nullptr && rows > 0 && columns > 0; }
};

// Vector structure
struct Vector {
    uint16_t size;
    float* data;
    bool isValid() const { return data != nullptr && size > 0; }
};

// Statistical data structure
struct StatisticalData {
    uint32_t count;
    float sum;
    float sumSquares;
    float min;
    float max;
    float mean;
    float variance;
    float standardDeviation;
    float median;
    float* sortedData;
};

// Filter configuration
struct FilterConfig {
    uint16_t order;
    float cutoffFrequency;
    float samplingFrequency;
    float dampingRatio;
    bool adaptive;
    float adaptationRate;
};

// Numerical algorithms configuration
struct NumericalAlgorithmsConfig {
    uint16_t maxMatrixSize;
    uint16_t maxVectorSize;
    uint16_t maxIterations;
    float convergenceTolerance;
    AlgorithmPrecision defaultPrecision;
    bool enableOptimization;
    bool enableParallelProcessing;
    uint16_t threadCount;
};

class NumericalAlgorithms {
private:
    // Configuration
    NumericalAlgorithmsConfig config;
    
    // Performance tracking
    AlgorithmPerformance lastPerformance;
    uint32_t totalExecutions;
    uint32_t successfulExecutions;
    uint32_t failedExecutions;
    
    // Error tracking
    uint8_t errorFlags;
    String lastError;
    
    // Helper methods
    bool validateMatrix(const Matrix& matrix) const;
    bool validateVector(const Vector& vector) const;
    bool validateDimensions(const Matrix& a, const Matrix& b) const;
    bool validateDimensions(const Matrix& matrix, const Vector& vector) const;
    
    void setError(const String& error);
    void updatePerformance(const AlgorithmPerformance& performance);
    float calculateAccuracy(float expected, float actual) const;

public:
    NumericalAlgorithms();
    ~NumericalAlgorithms();
    
    // Initialization and configuration
    bool initialize(const NumericalAlgorithmsConfig& config = NumericalAlgorithmsConfig{});
    void setConfiguration(const NumericalAlgorithmsConfig& newConfig);
    NumericalAlgorithmsConfig getConfiguration() const { return config; }
    
    // Matrix operations
    Matrix createMatrix(uint16_t rows, uint16_t columns);
    Matrix createIdentityMatrix(uint16_t size);
    Matrix createZeroMatrix(uint16_t rows, uint16_t columns);
    void destroyMatrix(Matrix& matrix);
    
    bool matrixAdd(const Matrix& a, const Matrix& b, Matrix& result);
    bool matrixSubtract(const Matrix& a, const Matrix& b, Matrix& result);
    bool matrixMultiply(const Matrix& a, const Matrix& b, Matrix& result);
    bool matrixScalarMultiply(const Matrix& matrix, float scalar, Matrix& result);
    bool matrixTranspose(const Matrix& matrix, Matrix& result);
    
    float matrixDeterminant(const Matrix& matrix);
    bool matrixInverse(const Matrix& matrix, Matrix& result);
    bool matrixLUDecomposition(const Matrix& matrix, Matrix& L, Matrix& U);
    bool matrixCholeskyDecomposition(const Matrix& matrix, Matrix& L);
    
    // Vector operations
    Vector createVector(uint16_t size);
    Vector createZeroVector(uint16_t size);
    void destroyVector(Vector& vector);
    
    bool vectorAdd(const Vector& a, const Vector& b, Vector& result);
    bool vectorSubtract(const Vector& a, const Vector& b, Vector& result);
    bool vectorScalarMultiply(const Vector& vector, float scalar, Vector& result);
    float vectorDotProduct(const Vector& a, const Vector& b);
    float vectorMagnitude(const Vector& vector);
    bool vectorNormalize(const Vector& vector, Vector& result);
    bool vectorCrossProduct(const Vector& a, const Vector& b, Vector& result);
    
    // Linear algebra solvers
    bool solveLinearSystem(const Matrix& A, const Vector& b, Vector& x);
    bool solveLinearSystemLU(const Matrix& A, const Vector& b, Vector& x);
    bool solveLinearSystemCholesky(const Matrix& A, const Vector& b, Vector& x);
    bool solveLinearSystemGaussSeidel(const Matrix& A, const Vector& b, Vector& x, uint16_t maxIterations = 100);
    bool solveLinearSystemJacobi(const Matrix& A, const Vector& b, Vector& x, uint16_t maxIterations = 100);
    
    // Eigenvalue and eigenvector calculations
    bool matrixEigenvalues(const Matrix& matrix, Vector& eigenvalues);
    bool matrixEigenvectors(const Matrix& matrix, Matrix& eigenvectors);
    bool matrixEigenDecomposition(const Matrix& matrix, Matrix& eigenvectors, Vector& eigenvalues);
    
    // Statistical algorithms
    StatisticalData calculateStatistics(const float* data, uint32_t count);
    StatisticalData calculateStatistics(const Vector& vector);
    
    float calculateMean(const float* data, uint32_t count);
    float calculateVariance(const float* data, uint32_t count);
    float calculateStandardDeviation(const float* data, uint32_t count);
    float calculateMedian(const float* data, uint32_t count);
    float calculateMode(const float* data, uint32_t count);
    float calculateSkewness(const float* data, uint32_t count);
    float calculateKurtosis(const float* data, uint32_t count);
    
    // Correlation and regression
    float calculateCorrelation(const float* x, const float* y, uint32_t count);
    float calculateCorrelation(const Vector& x, const Vector& y);
    bool calculateLinearRegression(const float* x, const float* y, uint32_t count, float& slope, float& intercept);
    bool calculateLinearRegression(const Vector& x, const Vector& y, float& slope, float& intercept);
    
    // Optimization algorithms
    float findMinimum(const float* data, uint32_t count);
    float findMaximum(const float* data, uint32_t count);
    uint32_t findMinimumIndex(const float* data, uint32_t count);
    uint32_t findMaximumIndex(const float* data, uint32_t count);
    
    bool gradientDescent(float (*function)(float), float startPoint, float learningRate, 
                        uint16_t maxIterations, float& result);
    bool newtonMethod(float (*function)(float), float (*derivative)(float), float startPoint,
                      uint16_t maxIterations, float& result);
    
    // Interpolation algorithms
    float linearInterpolation(float x1, float y1, float x2, float y2, float x);
    float bilinearInterpolation(float x, float y, float q11, float q12, float q21, float q22);
    float polynomialInterpolation(const float* x, const float* y, uint16_t count, float targetX);
    float splineInterpolation(const float* x, const float* y, uint16_t count, float targetX);
    
    // Integration algorithms
    float trapezoidalIntegration(float (*function)(float), float a, float b, uint16_t intervals);
    float simpsonIntegration(float (*function)(float), float a, float b, uint16_t intervals);
    float gaussianQuadrature(float (*function)(float), float a, float b, uint8_t order);
    
    // Differentiation algorithms
    float forwardDifference(float (*function)(float), float x, float h);
    float backwardDifference(float (*function)(float), float x, float h);
    float centralDifference(float (*function)(float), float x, float h);
    float richardsonExtrapolation(float (*function)(float), float x, float h);
    
    // Filtering algorithms
    float movingAverage(const float* data, uint16_t count, uint16_t windowSize);
    float exponentialMovingAverage(float input, float previous, float alpha);
    float kalmanFilter(float measurement, float previousEstimate, float processNoise, float measurementNoise);
    
    bool designLowPassFilter(float cutoffFreq, float samplingFreq, uint16_t order, float* coefficients);
    bool designHighPassFilter(float cutoffFreq, float samplingFreq, uint16_t order, float* coefficients);
    bool designBandPassFilter(float lowFreq, float highFreq, float samplingFreq, uint16_t order, float* coefficients);
    
    // Signal processing
    bool fft(const float* realInput, uint16_t size, float* realOutput, float* imagOutput);
    bool ifft(const float* realInput, const float* imagInput, uint16_t size, float* realOutput, float* imagOutput);
    bool convolution(const float* signal, uint16_t signalSize, const float* kernel, uint16_t kernelSize, float* result);
    bool correlation(const float* signal1, const float* signal2, uint16_t size, float* result);
    
    // Performance and monitoring
    AlgorithmPerformance getLastPerformance() const { return lastPerformance; }
    uint32_t getTotalExecutions() const { return totalExecutions; }
    uint32_t getSuccessfulExecutions() const { return successfulExecutions; }
    uint32_t getFailedExecutions() const { return failedExecutions; }
    float getSuccessRate() const;
    
    // Error handling
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // System operations
    void update();
    void shutdown();
    bool performSelfTest();
    bool isHealthy() const;
    
    // Utility methods
    void printConfiguration();
    void printPerformance();
    void printStatistics();
    void resetPerformance();
    
    // Memory management
    size_t getMemoryUsage() const;
    bool optimizeMemory();
    void cleanup();
};

#endif // NUMERICAL_ALGORITHMS_H 