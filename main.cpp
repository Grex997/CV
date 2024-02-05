#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>

// Function to read bounding boxes from a text file
void readBoundingBoxes(const std::string& filePath, std::vector<cv::Rect>& boundingBoxes, std::vector<int>& teamIDs) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        printf("Error: Could not open bounding box file: %s\n", filePath.c_str());
        return;
    }

    int x, y, width, height, teamID;
    while (file >> x >> y >> width >> height >> teamID) {
        boundingBoxes.push_back(cv::Rect(x, y, width, height));
        teamIDs.push_back(teamID);
    }

    file.close();
}

// Function to perform player and playing field segmentation using color-based segmentation
void colorBasedSegmentation(const cv::Mat& frame, cv::Mat& combinedMask, cv::Mat& fieldMask, const cv::Mat& playerBinMask, const cv::Mat& playerColMask) {
    // Convert the image to HSV color space
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    // Define color ranges for team 1 (Blue) and team 2 (Red) players
    cv::Scalar lowerTeam1(100, 100, 100);
    cv::Scalar upperTeam1(140, 255, 255);

    cv::Scalar lowerTeam2(0, 100, 100);
    cv::Scalar upperTeam2(10, 255, 255);

    // Create masks for team 1 and team 2 players
    cv::Mat team1Mask, team2Mask;
    cv::inRange(hsvImage, lowerTeam1, upperTeam1, team1Mask);
    cv::inRange(hsvImage, lowerTeam2, upperTeam2, team2Mask);

    // Combine masks to get a single mask highlighting players
    combinedMask = team1Mask | team2Mask;

    // Use the provided player binary and color masks
    cv::Mat playerBinaryMask = playerBinMask.clone();
    cv::Mat playerColorMask = playerColMask.clone();

    // Update the field mask using the provided player masks
    fieldMask = combinedMask & ~playerBinaryMask;
}
/*void colorBasedSegmentation(const cv::Mat& frame, cv::Mat& combinedMask, cv::Mat& fieldMask) {
    // Convert the image to HSV color space
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    // Define color ranges for team 1 (Blue) and team 2 (Red) players
    cv::Scalar lowerTeam1(100, 100, 100);
    cv::Scalar upperTeam1(140, 255, 255);

    cv::Scalar lowerTeam2(0, 100, 100);
    cv::Scalar upperTeam2(10, 255, 255);

    // Color range for playing field (you may need to adjust this based on your scenarios)
    cv::Scalar lowerField(40, 40, 40); // Example values for dark green
    cv::Scalar upperField(255, 255, 255);

    // Create masks for team 1, team 2 players, and playing field
    cv::Mat team1Mask, team2Mask;
    cv::inRange(hsvImage, lowerTeam1, upperTeam1, team1Mask);
    cv::inRange(hsvImage, lowerTeam2, upperTeam2, team2Mask);
    cv::inRange(hsvImage, lowerField, upperField, fieldMask);


    // Combine masks to get a single mask highlighting players and playing field
   // combinedMask = team1Mask | team2Mask | fieldMask;
    fieldMask = cv::Mat::zeros(frame.size(), CV_8U);
    cv::inRange(hsvImage, lowerField, upperField, fieldMask);
    combinedMask = fieldMask.clone();
}
*/
int main() {
    cv:: Mat fieldMask;
    // Loop through all images in the dataset
    for (int i = 1; i <= 15; ++i) {
        // Load the source image
        std::string imagePath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Images/im" + std::to_string(i) + ".jpg";
        cv::Mat frame = cv::imread(imagePath);

        if (frame.empty()) {
            printf("Error: Could not load image: %s\n", imagePath.c_str());
            return -1;
        }

        // Load bounding boxes
        std::vector<cv::Rect> boundingBoxes;
        std::vector<int> teamIDs;
        std::string bboxPath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Masks/im" + std::to_string(i) + "_bb.txt";
        // ..... Control it
        std::string binMaskFilePath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Masks/im" + std::to_string(i) + "_bin.png";
        std::string colorMaskFilePath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Masks/im" + std::to_string(i) + "_color.png";
        readBoundingBoxes(bboxPath, boundingBoxes, teamIDs);

       // Read ground truth binary segmentation mask
        cv::Mat groundTruthBinMask = cv::imread(binMaskFilePath, cv::IMREAD_GRAYSCALE);

        // Read ground truth color-coded segmentation mask
        cv::Mat groundTruthColorMask = cv::imread(colorMaskFilePath);

        // Perform player and playing field segmentation using color-based segmentation
        cv::Mat combinedMask, fieldMask;
        colorBasedSegmentation(frame, combinedMask, fieldMask, groundTruthBinMask, groundTruthColorMask);

        // Declare field mask
        //cv::Mat fieldMask;

        // Perform player and playing field segmentation using color-based segmentation
        //cv::Mat combinedMask;
        //colorBasedSegmentation(frame, combinedMask, fieldMask);

        // Display the result
        cv::Mat result = frame.clone();

        // Draw ground truth bounding boxes with lower opacity
        for (size_t j = 0; j < boundingBoxes.size(); ++j) {
            cv::Rect playerBox = boundingBoxes[j];
            int teamID = teamIDs[j];

            cv::Scalar color;
            if (teamID == 1) {
                color = cv::Scalar(255, 0, 0);  // Blue for team 1
            } else if (teamID == 2) {
                color = cv::Scalar(0, 0, 255);  // Red for team 2
            }

            // Draw filled rectangle with lower opacity
            cv::Mat playerOverlay = result.clone();
            cv::rectangle(playerOverlay, playerBox, color, cv::FILLED); // Fill the rectangle with color

            // Blend the original image with the filled rectangle
            cv::addWeighted(result, 0.5, playerOverlay, 0.5, 0.0, result);
        }

        //cv::Mat combinedMask;
        //colorBasedSegmentation(frame, combinedMask, fieldMask);

        // Overlay player and playing field segmentation on the result
        cv::Mat overlay = frame.clone();
        overlay.setTo(cv::Scalar(34, 128, 34));  // Set overlay to green for the playing field

        // control it ....

        // Overlay the playing field in green on the original image
        overlay.setTo(cv::Scalar(0, 255, 0), fieldMask); // Overlay in green

        // Overlay Team A players in blue
        cv::Mat teamAMask;
        cv::inRange(combinedMask, 1, 1, teamAMask);  // Extract Team A mask
        overlay.setTo(cv::Scalar(255, 0, 0), teamAMask);  // Overlay in blue

        // Overlay Team B players in red
        cv::Mat teamBMask;
        cv::inRange(combinedMask, 2, 2, teamBMask);  // Extract Team B mask
        overlay.setTo(cv::Scalar(0, 0, 255), teamBMask);  // Overlay in red

        // Display the results
        cv::imshow("Result", result);
        cv::imshow("Combined Segmentation", overlay);
        cv::imshow("groundTruthBinMask",groundTruthColorMask);
        cv::waitKey(0);
    }

    return 0;
}

/*#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>

// Function to read bounding boxes from a text file
void readBoundingBoxes(const std::string& filePath, std::vector<cv::Rect>& boundingBoxes, std::vector<int>& teamIDs) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        printf("Error: Could not open bounding box file: %s\n", filePath.c_str());
        return;
    }

    int x, y, width, height, teamID;
    while (file >> x >> y >> width >> height >> teamID) {
        boundingBoxes.push_back(cv::Rect(x, y, width, height));
        teamIDs.push_back(teamID);
    }

    file.close();
}

// Function to perform player and playing field segmentation using color-based segmentation
void colorBasedSegmentation(const cv::Mat& frame, cv::Mat& combinedMask ,cv::Mat& fieldMask) {
    // Example: Perform color-based segmentation to identify players and playing field
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    // Define color ranges for team 1 and team 2 players (you may need to fine-tune these)
    cv::Scalar lowerTeam1(100, 100, 50); // Example values for team 1 (Blue)
    cv::Scalar upperTeam1(140, 255, 255);

    cv::Scalar lowerTeam2(0, 100, 50); // Example values for team 2 (Red)
    cv::Scalar upperTeam2(10, 255, 255);

    // Color range for playing field (you may need to adjust this based on your scenarios)
    cv::Scalar lowerField(40, 40, 40); // Example values for dark green
    cv::Scalar upperField(255, 255, 255);

    // Create masks for team 1, team 2 players, and playing field
    cv::Mat team1Mask, team2Mask;
    cv::inRange(hsvImage, lowerTeam1, upperTeam1, team1Mask);
    cv::inRange(hsvImage, lowerTeam2, upperTeam2, team2Mask);
    cv::inRange(hsvImage, lowerField, upperField, fieldMask);

    // Combine masks to get a single mask highlighting players and playing field
    combinedMask = team1Mask | team2Mask | fieldMask;
    }

// Function to generate a grayscale mask based on the specified categories

// ... (calculateIoU function remains unchanged)

int main() {
    double totalAP = 0.0;
    double totalIoU = 0.0;

    // Loop through all images in the dataset
    for (int i = 1; i <= 15; ++i) {
        // Load the source image
        std::string imagePath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Images/im" + std::to_string(i) + ".jpg";
        cv::Mat frame = cv::imread(imagePath);

        if (frame.empty()) {
            printf("Error: Could not load image: %s\n", imagePath.c_str());
            return -1;
        }

        // Load bounding boxes
        std::vector<cv::Rect> boundingBoxes;
        std::vector<int> teamIDs;
        std::string bboxPath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Masks/im" + std::to_string(i) + "_bb.txt";
        // ..... Control it
        std::string binMaskFilePath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Masks/im" + std::to_string(i) + "_bin.png";
        std::string colorMaskFilePath = "C:/lezioni lm-25/Computer Vision/Lab/project/CV_24/Sport_scene_dataset/Masks/im" + std::to_string(i) + "_color.png";
        readBoundingBoxes(bboxPath, boundingBoxes, teamIDs);

        // Read ground truth binary segmentation mask
        cv::Mat groundTruthBinMask = imread(binMaskFilePath, cv::IMREAD_GRAYSCALE);

        // Read ground truth color-coded segmentation mask
        cv::Mat groundTruthColorMask = cv::imread(colorMaskFilePath);

        // Perform player and playing field segmentation using color-based segmentation
        cv::Mat combinedMask;
        colorBasedSegmentation(frame, combinedMask, fieldMask);

        // Display the result
        cv::Mat result = frame.clone();

        // Draw ground truth bounding boxes with lower opacity
        for (size_t j = 0; j < boundingBoxes.size(); ++j) {
            cv::Rect playerBox = boundingBoxes[j];
            int teamID = teamIDs[j];

            cv::Scalar color;
            if (teamID == 1) {
                color = cv::Scalar(255, 0, 0);  // Blue for team 1
            } else if (teamID == 2) {
                color = cv::Scalar(0, 0, 255);  // Red for team 2
            }

            // Draw filled rectangle with lower opacity
            cv::Mat playerOverlay = result.clone();
            cv::rectangle(playerOverlay, playerBox, color, cv::FILLED); // Fill the rectangle with color

            // Blend the original image with the filled rectangle
            cv::addWeighted(result, 0.5, playerOverlay, 0.5, 0.0, result);
        }

        // Overlay player and playing field segmentation on the result
        cv::Mat overlay = frame.clone();
        overlay.setTo(cv::Scalar(34, 128, 34));  // Set overlay to green for the playing field

        // Overlay players in blue
        cv::Mat playersMask;
        cv::inRange(combinedMask, 1, 2, playersMask);  // Extract players mask
        overlay.setTo(cv::Scalar(255, 0, 0), playersMask);  // Overlay players in blue

        // Overlay playing field in green
        overlay.setTo(cv::Scalar(0, 255, 0), fieldMask);  // Overlay playing field in green


        // Display the results
        cv::imshow("Result", result);
        cv::imshow("Combined Segmentation", overlay);
        cv::waitKey(0);
    }

    return 0;
}
*/
