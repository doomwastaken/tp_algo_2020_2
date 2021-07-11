#include <iostream>
#include <vector>
#include <array>
#include <cassert>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cstring>

const int SideSize = 4;
const int FieldSize = SideSize*SideSize;
const std::vector<char> WinState{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0};

class GameState{
 public:
    explicit GameState(const std::vector<char>& tmp);
    // explicit GameState(const GameState& tmp);
    GameState(const GameState &other) : field(other.field), emptyPos(other.emptyPos) {}

    int calcDistance()const;
    bool MoveRightAbility() const;
    bool MoveDownAbility() const;
    bool MoveUpAbility() const;
    bool MoveLeftAbility() const;
    GameState MoveRight()const;
    GameState MoveDown()const;
    GameState MoveLeft()const;
    GameState MoveUp()const;
    bool operator ==(const GameState& state)const {
        return field == state.field;
    }
    bool operator !=(const GameState& state)const {
        return !(*this == state);
    }
    bool operator <(const GameState& state)const {
        return calcDistance() > state.calcDistance();
    }
    friend struct GameStateHash;
    friend bool GetInversionCount(const GameState& state);

 private:
    std::vector<char> field;
    char emptyPos;
};

GameState::GameState(const std::vector<char>& tmp):field(tmp.size()) {
    for (size_t i = 0; i < tmp.size(); i++) {
        field[i] = tmp[i];
        if (tmp[i] == 0) {
            emptyPos = i;
        }
    }
}

int GameState::calcDistance() const {
    int distance = 0;
    for (size_t i = 0; i < FieldSize; i++) {
        if (field[i]) {
            distance += (abs(i % SideSize - (field[i] - 1) % SideSize) + abs(i / SideSize - (field[i] - 1) / SideSize));
        }
    }
    return distance;
}

bool GameState::MoveDownAbility() const {
    return emptyPos > SideSize;
}

bool GameState::MoveLeftAbility() const {
    return emptyPos %SideSize != SideSize-1;
}

bool GameState::MoveRightAbility() const {
    return emptyPos %SideSize != 0;
}

bool GameState::MoveUpAbility() const {
    return emptyPos <FieldSize - SideSize;
}

GameState GameState::MoveUp() const {
    assert(MoveUpAbility());
    GameState newState(*this);
    std::swap(newState.field[emptyPos], newState.field[emptyPos+SideSize]);
    newState.emptyPos +=SideSize;
    return newState;
}

GameState GameState::MoveRight() const {
    assert(MoveRightAbility());
    GameState newState(*this);
    std::swap(newState.field[emptyPos], newState.field[emptyPos - 1]);
    newState.emptyPos--;
    return newState;
}

GameState GameState::MoveLeft() const {
    assert(MoveLeftAbility());
    GameState newState(*this);
    std::swap(newState.field[emptyPos], newState.field[emptyPos+1]);
    newState.emptyPos++;
    return newState;
}

GameState GameState::MoveDown() const {
    assert(MoveDownAbility());
    GameState newState(*this);
    std::swap(newState.field[emptyPos], newState.field[emptyPos - SideSize]);
    newState.emptyPos -= SideSize;
    return newState;
}

struct GameStateHash {
 public:
    size_t operator()(const GameState & state) const {
        size_t hash = 0;
        memcpy(&hash, &state.field[0], sizeof(hash));
        return hash;
    }
};

bool GetInversionCount(const GameState& state) {
    int inv = 0;
    for (int i = 0; i < FieldSize; i++) {
        if (state.field[i]) {
            for (int j = 0; j < i; j++) {
                inv += state.field[j] > state.field[i]?1:0;
            }
        }
    }
    inv += 1 + state.emptyPos/4;
    return inv % 2 == 0;
}

std::string GetSolution(const std::vector<char>& field) {
    std::priority_queue<GameState> q;
    std::unordered_map<GameState, std::string, GameStateHash> visited;
    const GameState startState(field);
    if (!GetInversionCount(startState)) {
        return "";
    }
    visited[startState] = "";
    q.emplace(startState);
    GameState completed(WinState);
    while (q.top() != completed) {
        GameState curState = q.top();
        q.pop();
        if (curState.MoveLeftAbility()) {
            GameState leftState = curState.MoveLeft();
            if (visited.find(leftState) == visited.end()) {
                q.emplace(leftState);
                visited[leftState] = visited[curState] + "L";
            }
        }
        if (curState.MoveRightAbility()) {
            GameState rightState = curState.MoveRight();
            if (visited.find(rightState) == visited.end()) {
                q.emplace(rightState);
                visited[rightState] = visited[curState] + "R";
            }
        }
        if (curState.MoveUpAbility()) {
            GameState upState = curState.MoveUp();
            if ( visited.find(upState) == visited.end() ) {
                q.emplace(upState);
                visited[upState] = visited[curState] + "U";
            }
        }
        if (curState.MoveDownAbility()) {
            GameState downState = curState.MoveDown();
            if ( visited.find(downState) == visited.end() ) {
                q.emplace(downState);
                visited[downState] = visited[curState] + "D";
            }
        }
    }
    return visited[completed];
}

int main() {
    std::vector<char> arr(FieldSize);
    int s = 0;
    for (int i = 0; i < FieldSize; i++) {
        std::cin >> s;
        arr[i] = s;
    }
    std::string solution = GetSolution(arr);
    if (solution.empty()) {
        std::cout << -1 << std::endl;
        return 0;
    }
    std::cout << solution.size() << std::endl;
    for (char j : solution) {
        std::cout << j;
    }
    return 0;
}
