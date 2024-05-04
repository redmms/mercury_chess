QT       += core gui widgets multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#DEFINES += QT_MESSAGELOGCONTEXT

SOURCES += \
    ./app/fen_dialog.cpp \
    ./app/history_area.cpp \
    ./app/chat.cpp \
    ./app/local_types.cpp \
    ./app/mainwindow.cpp \
    ./app/rules_dialog.cpp \
    ./game/virtual_board.cpp \
    ./game/virtual_tile.cpp \
    ./game/virtual_validator.cpp \
    ./app/webclient.cpp \
    ./archiver/archiver.cpp \
    ./archiver/finestream/modules/bitremedy.ixx \
    ./archiver/finestream/modules/finestream.ixx \
    ./game/board.cpp \
    ./game/clock.cpp \
    ./game/tile.cpp \
    ./game/validator.cpp \
    ./main.cpp \
    ./app/offline_dialog.cpp \
    ./app/webclient_pack_tools.cpp \
    ./app/mainwindow_buttons.cpp  \
    ./app/debug_message_handler.cpp \

HEADERS += \
    ./app/chat.h \
    ./app/debug_message_handler.h \
    ./app/fen_dialog.h \
    ./app/local_types.h \
    ./app/mainwindow.h \
    ./app/history_area.h \
    ./app/rules_dialog.h \
    ./game/virtual_board.h \
    ./game/virtual_tile.h \
    ./game/virtual_validator.h \
    ./app/webclient.h \
    ./archiver/archiver.h \
    ./game/board.h \
    ./game/clock.h \
    ./game/tile.h \
    ./game/validator.h \
    ./app/offline_dialog.h \


FORMS += \
    ./app/fen_dialog.ui \
    ./app/mainwindow.ui \
    ./app/offline_dialog.ui \
    ./app/rules_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ./media.qrc

DISTFILES += \
    ./LICENSE.txt \
    ./images/bishop_black.svg \
    ./images/bishop_white.svg \
    ./images/exp.png \
    ./images/king_black.svg \
    ./images/king_white.svg \
    ./images/knight_black.svg \
    ./images/knight_white.svg \
    ./images/pawn_black.svg \
    ./images/pawn_white.svg \
    ./images/profile.png \
    ./images/queen_black.svg \
    ./images/queen_white.svg \
    ./images/rook_black.svg \
    ./images/rook_white.svg \
    ./images/temnozelenyi-zadnii-fon-100-foto-93.jpg \
    ./sounds/nice/wav/castling.wav \
    ./sounds/nice/wav/check.wav \
    ./sounds/nice/wav/check_to_opp.wav \
    ./sounds/nice/wav/eaten_by_opp.wav \
    ./sounds/nice/wav/eaten_by_user.wav \
    ./sounds/nice/wav/invalid_move.wav \
    ./sounds/nice/wav/lose.wav \
    ./sounds/nice/wav/move.wav \
    ./sounds/nice/wav/start.wav \
    ./sounds/nice/wav/promotion.wav \
    ./sounds/nice/wav/the_end_close_the_board.wav \
    ./sounds/nice/wav/win.wav \
    ./sounds/nice/wav/you_ate_ou-eee.wav
