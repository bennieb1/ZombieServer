#pragma once

#ifndef COMMON_H
#define COMMON_H

#include <./BitStream.h>
#include <RakNetTypes.h>
#include <MessageIdentifiers.h>


enum GameMessages {
    ID_GAME_MESSAGE_POSITION = ID_USER_PACKET_ENUM + 1,
    ID_GAME_MESSAGE_ROTATION
};

#endif // COMMON_H