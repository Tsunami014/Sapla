#pragma once
#include "note.hpp"
#include <deque>

void CLaddCard(FlashCard* newCard);
void CLclear();
extern std::vector<FlashCard*> allCards;

class GetFlashCard {
public:
    explicit GetFlashCard();
    ~GetFlashCard();

    void updateSchedule(int rating);
    void finish();

    // Forbid copying
    GetFlashCard(const GetFlashCard&) = delete;
    GetFlashCard& operator=(const GetFlashCard&) = delete;
    // Move constructor.
    GetFlashCard(GetFlashCard&& other) noexcept;
    // Move assignment.
    GetFlashCard& operator=(GetFlashCard&& other) noexcept;

    // Some operators, such as * and ->
    explicit operator bool() const;
    FlashCard& operator*() const;
    inline FlashCard* operator->() const { return *this ? ptr : nullptr; }
    inline FlashCard* get() const { return *this ? ptr : nullptr; }
    inline operator FlashCard*() const { return ptr; }

private:
    FlashCard* ptr;
    bool modify;
};

