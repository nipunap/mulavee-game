#pragma once

#include <ncurses.h>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <stdexcept>

namespace MulaWee {

// Modern enums for better type safety
enum class CellType : char {
    WALL = 0,
    PATH = 1,
    GOAL = 2
};

enum class GameState {
    MENU,
    PLAYING,
    LEVEL_COMPLETE,
    GAME_OVER,
    WINNER,
    QUIT
};

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Color pairs for ncurses
enum class ColorPair : int {
    RED = 1,
    GREEN = 2,
    BLUE = 3,
    YELLOW = 4,
    GOAL = 5,
    DEFAULT = 6
};

// Position structure for better coordinate handling
struct Position {
    int row, col;

    Position(int r = 0, int c = 0) : row(r), col(c) {}

    Position operator+(const Position& other) const {
        return Position(row + other.row, col + other.col);
    }

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

// Exception classes for better error handling
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& message)
        : std::runtime_error("Game Error: " + message) {}
};

class FileException : public GameException {
public:
    explicit FileException(const std::string& filename)
        : GameException("Failed to open file: " + filename) {}
};

// Level class - encapsulates level data and operations
class Level {
private:
    std::vector<std::vector<CellType>> grid;
    int rows, cols;
    Position goalPosition;
    std::string filename;

public:
    explicit Level(const std::string& levelFile);

    // Getters
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    const Position& getGoalPosition() const { return goalPosition; }
    CellType getCellType(const Position& pos) const;

    // Validation
    bool isValidPosition(const Position& pos) const;
    bool canMoveTo(const Position& pos) const;

    // Rendering
    void render(int startRow = 3, int startCol = 3) const;

private:
    void loadFromFile();
    char cellTypeToChar(CellType type) const;
};

// Player class - encapsulates player state and movement
class Player {
private:
    Position position;
    Position lastPosition;
    int moveCount;

public:
    Player(const Position& startPos = Position(20, 4))
        : position(startPos), lastPosition(startPos), moveCount(0) {}

    // Movement
    bool move(Direction dir, const Level& level);
    void setPosition(const Position& pos) { position = pos; }

    // Getters
    const Position& getPosition() const { return position; }
    const Position& getLastPosition() const { return lastPosition; }
    int getMoveCount() const { return moveCount; }

    // Reset for new level
    void reset(const Position& startPos);

    // Rendering
    void render();
    void clearLastPosition();

private:
    Position getDirectionOffset(Direction dir) const;
};

// Score management class
class ScoreManager {
private:
    std::string highScoreFile;
    std::string currentPlayerName;
    int currentScore;
    std::string highScorePlayerName;
    int highScore;

public:
    explicit ScoreManager(const std::string& scoreFile = "../data/score.dat");

    // Score operations
    void addLevelScore(int level, int moves);
    void setPlayerName(const std::string& name) { currentPlayerName = name; }

    // High score management
    void loadHighScore();
    void saveHighScore();
    bool isNewHighScore() const { return currentScore > highScore; }

    // Getters
    int getCurrentScore() const { return currentScore; }
    int getHighScore() const { return highScore; }
    const std::string& getCurrentPlayerName() const { return currentPlayerName; }
    const std::string& getHighScorePlayerName() const { return highScorePlayerName; }

    // Reset
    void resetScore() { currentScore = 0; }

    // Public method for calculating level scores
    int calculateLevelScore(int level, int moves) const;

private:
};

// RAII wrapper for ncurses - ensures proper cleanup
class NCursesWrapper {
private:
    bool initialized;

public:
    NCursesWrapper();
    ~NCursesWrapper();

    // Delete copy constructor and assignment operator
    NCursesWrapper(const NCursesWrapper&) = delete;
    NCursesWrapper& operator=(const NCursesWrapper&) = delete;

    void initializeColors();
    void cleanup();
};

// Main game class - orchestrates all game components
class Game {
private:
    std::unique_ptr<NCursesWrapper> ncursesWrapper;
    std::unique_ptr<ScoreManager> scoreManager;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Level>> levels;

    GameState currentState;
    int currentLevel;
    static constexpr int MAX_LEVELS = 3;

public:
    Game();
    ~Game() = default;

    // Main game loop
    void run();

private:
    // Game state management
    void initializeGame();
    void loadLevels();
    void handleMenuState();
    void handlePlayingState();
    void handleLevelCompleteState();
    void handleWinnerState();

    // Input handling
    void handlePlayerInput(int ch);
    Direction charToDirection(int ch) const;

    // UI rendering
    void renderGame();
    void renderUI();
    void renderHelp();
    void showWelcomeScreen();
    void showWinnerScreen();
    void showLevelCompleteScreen();

    // Game logic
    void startLevel(int level);
    void checkGoalReached();
    void nextLevel();
    bool askContinue();

    // Utility
    void clearScreen();
    void waitForKeyPress();
};

} // namespace MulaWee
