#include "optimized_game.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace MulaWee {

// Level class implementation
Level::Level(const std::string& levelFile) : rows(0), cols(0), filename(levelFile) {
    loadFromFile();
}

void Level::loadFromFile() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw FileException(filename);
    }

    file >> rows >> cols;
    if (rows <= 0 || cols <= 0 || rows > 100 || cols > 100) {
        throw GameException("Invalid level dimensions in " + filename);
    }

    grid.resize(rows, std::vector<CellType>(cols));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            char ch;
            file >> ch;

            switch (ch) {
                case '|':
                case '%':
                    grid[r][c] = CellType::WALL;
                    break;
                case '*':
                    grid[r][c] = CellType::PATH;
                    break;
                case '$':
                    grid[r][c] = CellType::GOAL;
                    goalPosition = Position(r, c);
                    break;
                default:
                    grid[r][c] = CellType::WALL; // Default to wall for unknown characters
                    break;
            }
        }
    }
}

CellType Level::getCellType(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return CellType::WALL;
    }
    return grid[pos.row][pos.col];
}

bool Level::isValidPosition(const Position& pos) const {
    return pos.row >= 0 && pos.row < rows && pos.col >= 0 && pos.col < cols;
}

bool Level::canMoveTo(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return false;
    }
    return grid[pos.row][pos.col] != CellType::WALL;
}

void Level::render(int startRow, int startCol) const {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            ::move(r + startRow, c + startCol);

            CellType cellType = grid[r][c];
            char ch = cellTypeToChar(cellType);

            switch (cellType) {
                case CellType::PATH:
                    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GREEN)));
                    printw(" ");
                    break;
                case CellType::GOAL:
                    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GOAL)));
                    printw("$");
                    break;
                case CellType::WALL:
                default:
                    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GREEN)));
                    printw("%c", ch);
                    break;
            }
        }
    }
}

char Level::cellTypeToChar(CellType type) const {
    switch (type) {
        case CellType::WALL: return '|';
        case CellType::PATH: return ' ';
        case CellType::GOAL: return '$';
        default: return '|';
    }
}

// Player class implementation
bool Player::move(Direction dir, const Level& level) {
    Position newPos = position + getDirectionOffset(dir);

    // Convert screen coordinates to grid coordinates (subtract rendering offset)
    Position gridPos(newPos.row - 3, newPos.col - 3);

    if (level.canMoveTo(gridPos)) {
        lastPosition = position;
        position = newPos;
        ++moveCount;
        return true;
    }

    // Invalid move - play beep sound
    beep();
    return false;
}

void Player::reset(const Position& startPos) {
    position = startPos;
    lastPosition = startPos;
    moveCount = 0;
}

void Player::render() {
    ::move(position.row, position.col);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("*");
}

void Player::clearLastPosition() {
    ::move(lastPosition.row, lastPosition.col);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GREEN)));
    printw(" ");
}

Position Player::getDirectionOffset(Direction dir) const {
    switch (dir) {
        case Direction::UP:    return Position(-1, 0);
        case Direction::DOWN:  return Position(1, 0);
        case Direction::LEFT:  return Position(0, -1);
        case Direction::RIGHT: return Position(0, 1);
        default: return Position(0, 0);
    }
}

// ScoreManager class implementation
ScoreManager::ScoreManager(const std::string& scoreFile)
    : highScoreFile(scoreFile), currentScore(0), highScore(0) {
    loadHighScore();
}

void ScoreManager::addLevelScore(int level, int moves) {
    int levelScore = calculateLevelScore(level, moves);
    currentScore += levelScore;
}

void ScoreManager::loadHighScore() {
    std::ifstream file(highScoreFile);
    if (file.is_open()) {
        file >> highScorePlayerName >> highScore;
    } else {
        // Set default values if file doesn't exist
        highScorePlayerName = "Default";
        highScore = 0;
    }
}

void ScoreManager::saveHighScore() {
    std::ofstream file(highScoreFile);
    if (file.is_open()) {
        file << currentPlayerName << " " << currentScore;
    }
}

int ScoreManager::calculateLevelScore(int level, int moves) const {
    // Original scoring algorithm from the game
    int baseScore;
    switch (level) {
        case 1: baseScore = 154; break;
        case 2: baseScore = 253; break;
        case 3: baseScore = 212; break;
        default: baseScore = 100; break;
    }

    // Score decreases with more moves, but never goes below 0
    int score = ((baseScore + std::max(0, baseScore - moves)) * baseScore) / 100;
    return std::max(0, score);
}

// NCursesWrapper class implementation
NCursesWrapper::NCursesWrapper() : initialized(false) {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    nodelay(stdscr, FALSE); // Wait for input
    initialized = true;
    initializeColors();
}

NCursesWrapper::~NCursesWrapper() {
    cleanup();
}

void NCursesWrapper::initializeColors() {
    if (has_colors()) {
        start_color();
        init_pair(static_cast<int>(ColorPair::RED), COLOR_RED, COLOR_BLACK);
        init_pair(static_cast<int>(ColorPair::GREEN), COLOR_GREEN, COLOR_BLACK);
        init_pair(static_cast<int>(ColorPair::BLUE), COLOR_BLUE, COLOR_BLACK);
        init_pair(static_cast<int>(ColorPair::YELLOW), COLOR_YELLOW, COLOR_BLACK);
        init_pair(static_cast<int>(ColorPair::GOAL), COLOR_BLACK, COLOR_YELLOW);
        init_pair(static_cast<int>(ColorPair::DEFAULT), COLOR_WHITE, COLOR_BLACK);
    }
}

void NCursesWrapper::cleanup() {
    if (initialized) {
        endwin();
        initialized = false;
    }
}

// Game class implementation
Game::Game() : currentState(GameState::MENU), currentLevel(0) {
    ncursesWrapper = std::make_unique<NCursesWrapper>();
    scoreManager = std::make_unique<ScoreManager>();
    player = std::make_unique<Player>();
}

void Game::run() {
    try {
        initializeGame();

        while (currentState != GameState::QUIT) {
            switch (currentState) {
                case GameState::MENU:
                    handleMenuState();
                    break;
                case GameState::PLAYING:
                    handlePlayingState();
                    break;
                case GameState::LEVEL_COMPLETE:
                    handleLevelCompleteState();
                    break;
                case GameState::WINNER:
                    handleWinnerState();
                    break;
                case GameState::GAME_OVER:
                case GameState::QUIT:
                    return;
            }
        }
    } catch (const GameException& e) {
        ncursesWrapper->cleanup();
        std::cerr << e.what() << std::endl;
    } catch (const std::exception& e) {
        ncursesWrapper->cleanup();
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void Game::initializeGame() {
    loadLevels();
    currentState = GameState::MENU;
}

void Game::loadLevels() {
    levels.clear();
    levels.reserve(MAX_LEVELS);

    for (int i = 1; i <= MAX_LEVELS; ++i) {
        std::string filename = "../data/level" + std::to_string(i) + ".dat";
        levels.push_back(std::make_unique<Level>(filename));
    }
}

void Game::handleMenuState() {
    showWelcomeScreen();

    // Get player name
    echo();
    char nameBuffer[11];
    ::move(20, 60);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("Enter your name: ");
    getnstr(nameBuffer, 10);
    noecho();

    scoreManager->setPlayerName(std::string(nameBuffer));
    scoreManager->resetScore();
    currentLevel = 0;
    currentState = GameState::PLAYING;
    startLevel(currentLevel);
}

void Game::handlePlayingState() {
    // Render the game once when entering this state
    renderGame();

    // Main game loop - keep getting input until quit or level complete
    int ch;
    while (currentState == GameState::PLAYING) {
        ch = getch();

        if (ch == 'q' || ch == 'Q') {
            currentState = GameState::QUIT;
            return;
        }

        handlePlayerInput(ch);
        checkGoalReached();
    }
}

void Game::handleLevelCompleteState() {
    showLevelCompleteScreen();

    // Add score for completed level
    scoreManager->addLevelScore(currentLevel + 1, player->getMoveCount());

    if (currentLevel + 1 >= MAX_LEVELS) {
        currentState = GameState::WINNER;
    } else {
        nextLevel();
        currentState = GameState::PLAYING;
    }
}

void Game::handleWinnerState() {
    showWinnerScreen();

    // Save high score if it's a new record
    if (scoreManager->isNewHighScore()) {
        scoreManager->saveHighScore();
    }

    if (askContinue()) {
        currentState = GameState::MENU;
    } else {
        currentState = GameState::QUIT;
    }
}

void Game::handlePlayerInput(int ch) {
    Direction dir;

    try {
        dir = charToDirection(ch);
    } catch (const GameException&) {
        // Invalid key pressed
        beep();
        ::move(levels[currentLevel]->getRows() + 6, 3);
        attrset(COLOR_PAIR(static_cast<int>(ColorPair::RED)));
        printw("'%c' is Invalid Key.... (code: %d)", ch, ch);
        refresh();
        return;
    }

    // Show that we received valid input
    ::move(levels[currentLevel]->getRows() + 6, 3);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GREEN)));
    printw("Key pressed: %c                    ", ch);

    // Try to move player
    if (player->move(dir, *levels[currentLevel])) {
        // Movement successful - clear old position and render at new position
        player->clearLastPosition();
        player->render();

        // Update the UI to show new move count
        renderUI();
        refresh(); // Update the screen immediately
    } else {
        // Movement failed - re-render player at current position
        player->render();
        refresh();
    }
}

Direction Game::charToDirection(int ch) const {
    switch (ch) {
        case 'w': case 'W': return Direction::UP;
        case 's': case 'S': return Direction::DOWN;
        case 'a': case 'A': return Direction::LEFT;
        case 'd': case 'D': return Direction::RIGHT;
        default:
            throw GameException("Invalid direction key");
    }
}

void Game::renderGame() {
    clearScreen();

    // Render header
    ::move(1, 30);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::RED)));
    printw("MULA WEE (Optimized Version 2.0)");

    ::move(2, 3);
    printw("Level: %d", currentLevel + 1);

    // Render level
    levels[currentLevel]->render();

    // Render player
    player->render();

    // Render UI
    renderUI();

    refresh();
}

void Game::renderUI() {
    int uiRow = levels[currentLevel]->getRows() + 4;

    ::move(uiRow, 3);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::BLUE)));
    printw("Position: (%d, %d)",
           player->getPosition().row - 3,
           player->getPosition().col - 3);

    ::move(uiRow + 1, 3);
    printw("Moves: %d", player->getMoveCount());

    ::move(uiRow + 2, 3);
    printw("Score: %d", scoreManager->getCurrentScore());

    ::move(uiRow + 3, 3);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("Controls: WASD to move, Q to quit");
}

void Game::renderHelp() {
    int helpRow = levels[currentLevel]->getRows() + 8;

    ::move(helpRow, 10);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("ATTENTION! Navigate to the yellow box ($) to win!");

    ::move(helpRow + 1, 10);
    printw("Use WASD keys to move. Avoid walls (|, %%).");
}

void Game::showWelcomeScreen() {
    clearScreen();

    // Draw border
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::RED)));
    for (int i = 2; i < 72; i++) {
        mvprintw(2, i, "*");
        mvprintw(22, i, "*");
    }
    for (int i = 2; i < 23; i++) {
        mvprintw(i, 2, "*");
        mvprintw(i, 72, "*");
    }

    // Game title
    ::move(6, 30);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("MULA WEE");

    ::move(7, 25);
    printw("Optimized Version 2.0");

    // Instructions
    ::move(10, 8);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GREEN)));
    printw("Navigate through the maze to reach the goal ($)");

    ::move(12, 8);
    printw("Controls:");
    ::move(13, 12);
    printw("W - Move Up");
    ::move(14, 12);
    printw("A - Move Left");
    ::move(15, 12);
    printw("S - Move Down");
    ::move(16, 12);
    printw("D - Move Right");
    ::move(17, 12);
    printw("Q - Quit Game");

    // High score
    ::move(19, 8);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::BLUE)));
    printw("High Score: %s - %d",
           scoreManager->getHighScorePlayerName().c_str(),
           scoreManager->getHighScore());

    refresh();
}

void Game::showWinnerScreen() {
    clearScreen();

    // Draw decorative border
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::RED)));
    for (int i = 2; i < 71; i++) {
        for (int j = 2; j < 21; j++) {
            if ((i > 4 && i < 71) && (j == 2 || j == 20)) {
                mvprintw(j, i, "*");
            } else if ((i == 2 || i == 70) && (j < 19 && j > 3)) {
                mvprintw(j, i, "*");
            }
        }
    }

    // Winner message
    ::move(5, 32);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("---MULA WEE---");

    ::move(7, 20);
    printw("YOU ARE THE WINNER!");

    // Score display
    ::move(10, 20);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GREEN)));
    if (scoreManager->isNewHighScore()) {
        printw("NEW HIGH SCORE!");
        ::move(11, 20);
        printw("%s: %d",
               scoreManager->getCurrentPlayerName().c_str(),
               scoreManager->getCurrentScore());
    } else {
        printw("Your Score: %d", scoreManager->getCurrentScore());
        ::move(11, 20);
        printw("High Score: %s - %d",
               scoreManager->getHighScorePlayerName().c_str(),
               scoreManager->getHighScore());
    }

    // Credits
    ::move(16, 10);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::BLUE)));
    printw("Original by: Nipuna Perera (2004)");
    ::move(17, 10);
    printw("Optimized Version: 2024");

    ::move(19, 20);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("Press any key to continue...");

    refresh();
    getch();
}

void Game::showLevelCompleteScreen() {
    clearScreen();

    ::move(8, 25);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::GREEN)));
    printw("Level %d Complete!", currentLevel + 1);

    ::move(10, 25);
    printw("Moves: %d", player->getMoveCount());

    ::move(11, 25);
    printw("Level Score: %d",
           scoreManager->calculateLevelScore(currentLevel + 1, player->getMoveCount()));

    ::move(12, 25);
    printw("Total Score: %d", scoreManager->getCurrentScore());

    if (currentLevel + 1 < MAX_LEVELS) {
        ::move(15, 25);
        attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
        printw("Preparing Level %d...", currentLevel + 2);
    } else {
        ::move(15, 25);
        attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
        printw("All levels complete! Calculating final score...");
    }

    ::move(20, 25);
    printw("Press any key to continue...");

    refresh();
    getch();
}

void Game::startLevel(int level) {
    if (level < 0 || level >= MAX_LEVELS) {
        throw GameException("Invalid level number");
    }

    currentLevel = level;

    // Reset player position for the new level
    Position startPos(20, 4); // Default starting position
    player->reset(startPos);
}

void Game::checkGoalReached() {
    Position playerPos = player->getPosition();
    Position adjustedPos(playerPos.row - 3, playerPos.col - 3); // Adjust for rendering offset

    // Check if player is on a goal cell (same logic as original game)
    CellType cellType = levels[currentLevel]->getCellType(adjustedPos);
    if (cellType == CellType::GOAL) {
        currentState = GameState::LEVEL_COMPLETE;
    }
}

void Game::nextLevel() {
    if (currentLevel + 1 < MAX_LEVELS) {
        startLevel(currentLevel + 1);
    }
}

bool Game::askContinue() {
    clearScreen();

    ::move(10, 30);
    attrset(COLOR_PAIR(static_cast<int>(ColorPair::YELLOW)));
    printw("Play again? (y/n): ");

    char ch;
    do {
        ch = getch();
    } while (ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N');

    return (ch == 'y' || ch == 'Y');
}

void Game::clearScreen() {
    clear();
}

void Game::waitForKeyPress() {
    getch();
}

} // namespace MulaWee
