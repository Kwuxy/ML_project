#include "library.h"
#include <chrono>
#include <random>

#include <iostream>

//[type_model][class/regression][fonction]

extern "C" {
    __declspec(dllexport) double * linearCreateModel(int inDim) {
        int weightsSize = inDim + 1;
        auto weights = (double *)(malloc(weightsSize * sizeof(double)));

        std::default_random_engine randomEngine(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> distribution{-1, 1};

        for(int i = 0; i < weightsSize; i++) {
            weights[i] = distribution(randomEngine);
        }

        return weights;
    }

    __declspec(dllexport) int linearClassPredict(double *model, int inDim, double *params) {
        double sum = model[0];
        for(int i = 0; i < inDim; i++) {
            sum += params[i] * model[i + 1];
        }

        int result = sign(sum);
        return result;
    }

    __declspec(dllexport)void linearClassTrain(double *model, int inDim, int epoch, double trainingStep,
            double *trainingParams, int trainingParamsNumber, const double *trainingResults) {

        std::default_random_engine randomEngine(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> distribution{0, 1};

        for(int e = 0; e < epoch; e++) {
            int trainingPicked = floor(distribution(randomEngine) * trainingParamsNumber);
            int trainingParamsPosition = inDim * trainingPicked;
            double modification = (double)trainingStep * (trainingResults[trainingPicked] -
                    linearClassPredict(model, inDim, &trainingParams[trainingParamsPosition]));
            model[0] += modification;

            for(int j = 0; j < inDim; j++) {
                model[j + 1] += modification * trainingParams[trainingParamsPosition + j];
            }
        }
    }

    __declspec(dllexport) void linearClearModel(const double *model) {
        delete model;
    }

    __declspec(dllexport) double * mlpCreateModel(int layers, const int npl[]) {
        int totalNodes = 0;
        for(int i = 0; i < layers - 1; i++) {
            totalNodes += (npl[i] + 1) * npl[i + 1];
        }

        int modelSize = 1 + layers + totalNodes;
        auto model = (double *)(malloc(modelSize * sizeof(double)));

        model[0] = layers;
        int modelIndex = 1;
        for(int i = 0; i < layers; i++) {
            model[modelIndex] = npl[i];
            modelIndex++;
        }

        std::default_random_engine randomEngine(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> distribution{-1, 1};

        while(modelIndex < modelSize) {
            model[modelIndex] = distribution(randomEngine);
            modelIndex++;
        }

        return model;
    }
}

int sign(double value) {
    return value == 0 ? 0 : value < 0 ? -1 : 1;
}

void predictAll(double *model) {
    for(double i = -7; i < 8; i += 1) {
        for(double j = 0; j < 15; j += 1) {
            double params[] = {i, j};
            int predicted = linearClassPredict(model, 2, params);
            std::cout << (predicted == 0 ? "0" : predicted >= 1 ? "+" : "-");
        }
        std::cout << std::endl;
    }
}

int main(int argc, char **argv) {
    int inDim = 2;
    double *model = linearCreateModel(inDim);

    double trainingParams[] = {-3, 9, 6, 13, -7, 2};
    double trainingResults[] = {1, 1, -1};
    linearClassTrain(model, 2, 1000, 0.1, trainingParams, 3, trainingResults);

    predictAll(model);

    return 0;
}