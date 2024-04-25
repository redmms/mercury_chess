QT       += core gui widgets multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#DEFINES += QT_MESSAGELOGCONTEXT

SOURCES += \
    ../src/app/fen_dialog.cpp \
    ../src/app/history_area.cpp \
    ../src/app/chat.cpp \
    ../src/app/local_types.cpp \
    ../src/app/mainwindow.cpp \
    ../src/app/rules_dialog.cpp \
    ../src/game/virtual_board.cpp \
    ../src/game/virtual_tile.cpp \
    ../src/game/virtual_validator.cpp \
    ../src/app/webclient.cpp \
    ../src/archiver/archiver.cpp \
    ../src/archiver/bitremedy.ixx \
    ../src/archiver/finestream.ixx \
    ../src/game/board.cpp \
    ../src/game/clock.cpp \
    ../src/game/tile.cpp \
    ../src/game/validator.cpp \
    ../src/main.cpp \
    ../src/app/offline_dialog.cpp \
    ../src/app/mainwindow_buttons.cpp \
    ../src/app/webclient_pack_tools.cpp

HEADERS += \
    ../src/app/chat.h \
    ../src/app/debug_message_handler.hpp \
    ../src/app/fen_dialog.h \
    ../src/app/local_types.h \
    ../src/app/mainwindow.h \
    ../src/app/history_area.h \
    ../src/app/rules_dialog.h \
    ../src/game/virtual_board.h \
    ../src/game/virtual_tile.h \
    ../src/game/virtual_validator.h \
    ../src/app/webclient.h \
    ../src/archiver/archiver.h \
    ../src/game/board.h \
    ../src/game/clock.h \
    ../src/game/tile.h \
    ../src/game/validator.h \
    ../src/app/offline_dialog.h

FORMS += \
    ../src/app/fen_dialog.ui \
    ../src/app/mainwindow.ui \
    ../src/app/offline_dialog.ui \
    ../src/app/rules_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../src/media.qrc

DISTFILES += \
    ../src/LICENSE.txt \
    ../src/images/bishop_black.svg \
    ../src/images/bishop_white.svg \
    ../src/images/exp.png \
    ../src/images/king_black.svg \
    ../src/images/king_white.svg \
    ../src/images/knight_black.svg \
    ../src/images/knight_white.svg \
    ../src/images/pawn_black.svg \
    ../src/images/pawn_white.svg \
    ../src/images/profile.png \
    ../src/images/queen_black.svg \
    ../src/images/queen_white.svg \
    ../src/images/rook_black.svg \
    ../src/images/rook_white.svg \
    ../src/images/temnozelenyi-zadnii-fon-100-foto-93.jpg \
    ../src/sounds/nice/wav/castling.wav \
    ../src/sounds/nice/wav/check.wav \
    ../src/sounds/nice/wav/check_to_opp.wav \
    ../src/sounds/nice/wav/eaten_by_opp.wav \
    ../src/sounds/nice/wav/eaten_by_user.wav \
    ../src/sounds/nice/wav/invalid_move.wav \
    ../src/sounds/nice/wav/lose.wav \
    ../src/sounds/nice/wav/move.wav \
    ../src/sounds/nice/wav/start.wav \
    ../src/sounds/nice/wav/promotion.wav \
    ../src/sounds/nice/wav/the_end_close_the_board.wav \
    ../src/sounds/nice/wav/win.wav \
    ../src/sounds/nice/wav/you_ate_ou-eee.wav
