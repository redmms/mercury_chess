#include "mainwindow.h"
#include "webclient.h"
#include "offline_dialog.h"
#include "rules_dialog.h"
#include "history_area.h"
#include "chat.h"
#include "fen_dialog.h"
#include "debug_message_handler.h"
#include "../game/board.h"
#include "../archiver/archiver.h"
#include <QFileDialog>
#include <QCryptographicHash>
#include <QClipboard>
#include <QDesktopServices>
#include <QDebug>
#ifdef MMDTEST
#include <iostream>
#endif  // MMDTEST
using namespace std;

namespace mmd 
{
    void MainWindow::on_draw_button_clicked()
    {
        net->sendToServer(packnum::draw_suggestion);
    }

    void MainWindow::on_resign_button_clicked()
    {
        net->sendToServer(packnum::resignation);
        endSlot(endnum::user_resignation);
    }

    void MainWindow::on_actionProfile_triggered()
    {
        openTab(ui->settings_tab);
    }

    void MainWindow::on_change_photo_button_clicked()
    {
        // open dialog window to choose a photo
        int width = settings[pic_w_e].toInt();
        int height = settings[pic_h_e].toInt();
        QString w = QString::number(width);
        QString h = QString::number(height);
        QString avatar_address = QFileDialog::getOpenFileName(this, "Open File",
            QString("Choose a photo for avatar. Avatar picture will be scaled to " + w + "x" + h + " pixel image."),
            tr("Images (*.png *.jpg *.jpeg *.pgm)"));
        if (!avatar_address.isEmpty()) {
            QPixmap user_pic = QPixmap(avatar_address).scaled(width, height);
            setPic(user_pic_e, user_pic);
            ui->profile_avatar->setPixmap(user_pic);
            if (game_active)
                ui->user_avatar->setPixmap(user_pic);
        }
    }

    void MainWindow::on_change_name_button_clicked()
    {
        QString new_name = ui->name_edit->text();
        ui->name_edit->clear();
        int err = changeLocalName(new_name);
        if (err == 1) {
            showBox("Embarrasing!",
                "This nickname is too small. It can't be empty.",
                QMessageBox::Warning);
        }
        else if (err == 2) {
            int max_nick = settings[max_nick_e].toInt();
            showBox("So huge!",
                "This nickname is too long. Maximum length is " + QString::number(max_nick),
                QMessageBox::Warning);
        }
        else {
            if (net) {
                net->sendToServer(packnum::new_name, 0, new_name);
            }
            else {
                showBox("Can't change online nickname",
                    "You are offline, only local nickname has been changed. "
                    "To change online nickname you will need to enter account. ");
            }
        }
    }

    void MainWindow::on_back_from_settings_clicked()
    {
        openTab(last_tab);
    }

    void MainWindow::on_actionWith_friend_triggered()
    {
        QString game_regime = settings[game_regime_e].toString();
        openTab(ui->friend_connect_tab);
    }

    void MainWindow::on_login_button_clicked()
    {
        openTab(ui->login_tab);
        login_regime = log_r;
    }

    void MainWindow::on_registrate_button_clicked()
    {
        openTab(ui->login_tab);
        login_regime = reg_r;
    }

    void MainWindow::on_end_login_button_clicked()
    {
        QString entered_username = ui->login_edit->text();
        int max_nick = settings[max_nick_e].toInt();
        if (ui->login_password->text().isEmpty()) {
            showBox("Too small for the party!",
                "You need to enter a login",
                QMessageBox::Warning);
        }
        else if (entered_username.isEmpty()) {
            showBox("Too small for the party!",
                "You need to enter a password",
                QMessageBox::Warning);
        }
        else if (entered_username.size() > max_nick) {
            showBox("So huge!",
                "This nickname is too long. Maximum length is " + QString::number(max_nick),
                QMessageBox::Warning);
        }
        else if (ui->login_password->text().size() < 6) {
            showBox("Too small for the party!",
                "The password should be at least 7 symbols long",
                QMessageBox::Warning);
        }
        else {
            changeLocalName(entered_username);
            QCryptographicHash hasher(QCryptographicHash::Sha512);
            hasher.addData(ui->login_password->text().toLocal8Bit());
            QByteArray hashed_password = hasher.result();
            settings[user_pass_e].setValue(hashed_password);
            if (login_regime == reg_r)
                net->sendToServer(packnum::registration);
            else if (login_regime == log_r)
                net->sendToServer(packnum::login);
        }
    }

    void MainWindow::on_back_from_login_button_clicked()
    {
        openTab(ui->pre_tab);
    }

    void MainWindow::on_guest_button_clicked()
    {
        int max_nick = settings[max_nick_e].toInt();
        login_regime = guest_r;
        QString random_username = "Lazy" + QString::number(rand() % (int)pow(10, max_nick - 4));
        changeLocalName(random_username);
        net->sendToServer(packnum::registration);
        net->sendToServer(packnum::login);
    }

    void MainWindow::on_send_invite_button_clicked()
    {
        int chosen_time = settings[time_setup_e].toInt();
        QString opp_name = ui->friend_name_edit->text();
        QString user_name = settings[user_name_e].toString();
        if (!chosen_time) {
            showBox("Set up match timer",
                "You need to choose initial time for chess clock.",
                QMessageBox::Warning);
        }
        else if (opp_name.isEmpty()) {
            showBox("Enter friend's name",
                "You need to choose friend's name.",
                QMessageBox::Warning);
        }
        else if (opp_name == user_name) {
            showBox("Too cunning",
                "You can't send invite to yourself",
                QMessageBox::Warning);
        }
        else {
            bool match_side = rand() % 2;
            settings[match_side_e].setValue(match_side);
            settings[opp_name_e].setValue(opp_name);
            net->sendToServer(invite);
        }
    }

    void MainWindow::editReturnSlot()
    {
        QString message_text = ui->message_edit->toPlainText();
        chat->printMessage(settings[user_name_e].toString(), true, message_text);
        ui->message_edit->clear();
        if (settings[game_regime_e].toString() == friend_online)
            net->sendToServer(chat_message, false, message_text);
    }

    bool MainWindow::eventFilter(QObject* object, QEvent* event)
    {
        if (object == ui->message_edit && ui->message_edit->hasFocus() && event->type() == QEvent::KeyPress) {
            QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
            if (key_event && (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter)) {
                emit editReturnPressed();
                return true;
            }
            else {
                return QMainWindow::eventFilter(object, event);
            }
        }
        else {
            return QMainWindow::eventFilter(object, event);
        }
    }

    void MainWindow::on_actionToggle_fullscreen_triggered()
    {
        static bool fullscreen = false;
        if (fullscreen) {
            showMaximized();
            if (game_active)
                statusBar()->show();
        }
        else {
            showFullScreen();
            if (game_active)
                statusBar()->hide();
        }
        fullscreen = !fullscreen;
    }

    void MainWindow::on_actionWith_friend_offline_triggered()
    {
        OfflineDialog dialog(this);
        if (dialog.exec() == QDialog::Rejected) {
            setPic(opp_pic_e, getPic(def_pic_e));
            settings[opp_name_e].setValue("Friend"_qs);
        }
        startGame(friend_offline);
    }

    void MainWindow::on_actionEnter_triggered()
    {
        openTab(ui->pre_tab);
    }

    void MainWindow::on_change_ip_button_clicked()
    {
        QString new_address = ui->ip_edit->text();
        if (new_address.isEmpty()) {
            showBox("Empty address", "Can't be empty");
            return;
        }
        QString new_port_str = ui->port_edit->text();
        if (new_port_str.isEmpty()) {
            showBox("Empty port", "Can't be empty");
            return;
        }
        int new_port = new_port_str.toInt();
        settings[ip_address_e].setValue(new_address);
        settings[port_address_e].setValue(new_port);
        if (net) {
            net->connectNewHost();
        }
        else {
            showBox("Success", "New adress saved locally. 'net' variable not initialized.");
        }
    }

    void MainWindow::on_restore_default_button_clicked()
    {
        QString def_address = settings[def_address_e].toString();
        settings[ip_address_e].setValue(def_address);
        int def_port = settings[def_port_e].toInt();
        settings[port_address_e].setValue(def_port);
        if (net)
            net->connectNewHost();
        ui->ip_edit->clear();
        ui->port_edit->clear();
    }

    void MainWindow::my_offline_stop_button_clicked()
    {
        endSlot(endnum::interrupt);
    }

    void MainWindow::my_offline_back_button_clicked()
    {
        openTab(last_tab);
    }

    void MainWindow::my_history_next_button_clicked()
    {
        board->moveForward();
    }

    void MainWindow::my_history_previous_button_clicked()
    {
        board->moveBack();
    }

    void MainWindow::on_actionAbout_triggered()
    {
        QString title = "About Mercury Chess";
        QString description = "I am Max Cury (MMD18) and this is my pet project written in C++ programming language with Qt framework.<br><br>"
            "It may seem that the main feature of the program is a chess game with a beautiful style, but it is absolutely not.<br><br>The main feature is its built-in archiver "
            "with its own chess data format using a sophisticated algorithm to compress the data, so that it takes even less than 1 byte of space for every halfmove and uses its own "
            "bitstream allowing it to operate individual bits in cout style (available on <a href = 'https://github.com/redmms/finestream.git'>github</a>).<br><br>I also invented several algorithms for describing chess board positions "
            "(like FEN but with a different structure and encoded in bits) and will code them if I find some "
            "interest of programmers or chess community. "
            "<br><br>"
            "To contact me, use this email: mmd18cury@yandex.ru";
        QMessageBox msg_box(this);
        msg_box.setWindowTitle(title);
        msg_box.setTextFormat(Qt::RichText);   //this is what makes the links clickable
        msg_box.setText(description);
        msg_box.exec();
    }


    void MainWindow::on_actionAbout_Qt_triggered()
    {
        QMessageBox::aboutQt(this, "About Qt");
    }

    void MainWindow::on_actionRules_triggered()
    {
        RulesDialog* dialog = new RulesDialog(this);
        connect(dialog, &RulesDialog::finished, [dialog]() {
            dialog->deleteLater();
            });
        dialog->show();
    }

    void MainWindow::on_actionSend_suggestion_triggered()
    {
        QDesktopServices::openUrl(QUrl("mailto:mmd18cury@yandex.ru?subject=MercuryChess:Suggestion&body=Please, make it impossible to lose.", QUrl::TolerantMode));
    }

    void MainWindow::on_actionReport_a_bug_triggered()
    {
        LogHandler::log_ofstream.flush();
        QString app_dir = QCoreApplication::applicationDirPath();
        QDesktopServices::openUrl(QUrl("mailto:mmd18cury@yandex.ru?subject=MercuryChess:Bug report&body=Please attach the log file. Choose to attach a file and enter next path, then choose log.txt file: " + app_dir, QUrl::TolerantMode));
    }

    void MainWindow::on_actionSave_game_triggered()
    {
        if (!board) {
            showBox("Nothing to save",
                "You need to have an open game to save to use this option.");
            return;
        }
        QString game_regime = settings[game_regime_e].toString();
        if (game_regime == training) {
            showBox("In development", "Not available for training mode yet.");
            return;
        }

        // Create directory
        QString app_dir = QCoreApplication::applicationDirPath();
        QString archive_dir = app_dir + "/saved_games";
        QDir dir;
        if (!dir.exists(archive_dir)) {
            if (dir.mkdir(archive_dir)) {
                qDebug() << "saved_games folder created";
            }
            else {
                qDebug() << "Couldn't create saved_games folder";
            }
        }

        // Create archive file
        QString archive_fullname =
            archive_dir
            + "/"
            + settings[opp_name_e].toString()
            + "_"
            + curTime()
            + ".mmd18";
        QFileDialog save_dialog(this,
            "Save File",
            archive_fullname,
            tr("Chess Archive (*.mmd18)"));
        save_dialog.setAcceptMode(QFileDialog::AcceptSave);
        save_dialog.setFileMode(QFileDialog::AnyFile);
        QString selected_fullname;
        if (save_dialog.exec()) {
            selected_fullname = save_dialog.selectedFiles().first();
        }
        else {
            return;
        }
        if (!selected_fullname.isEmpty()) {
            archive_fullname = selected_fullname;
        }
        QFile archive(archive_fullname);
        if (archive.open(QIODevice::WriteOnly | QIODevice::Text)) {
            archive.close();
            qDebug() << "Archive file" << archive_fullname << "successfuly created.";
        }
        else {
            qDebug() << "Couldn't open archive file" << archive_fullname;
        }

        // Write game to the file
        Archiver archiver;
        int error = archiver.writeGame(board->EndType(), board->Bistory(), archive_fullname);
        if (!error) {
            showBox("Good news",
                "Operation done successfuly.");
        }
        else {
            showBox("Oops",
                "Something went wrong. Error code: " + QString::number(error));
        }
    }

    void MainWindow::on_actionLoad_game_triggered()
    {
        QString app_dir = QCoreApplication::applicationDirPath();
        QString archive_dir = app_dir + "/saved_games";

        // Create archive file
        QString archive_fullname = QFileDialog::getOpenFileName(this,
            "Open File",
            archive_dir,
            tr("Chess Archive (*.mmd18)"));
        if (archive_fullname.isEmpty()) {
            return;
        }
        QFile archive(archive_fullname);
        if (archive.open(QIODevice::ReadOnly | QIODevice::Text)) {
            archive.close();
            qDebug() << "Archive file" << archive_fullname << "is ready to be open.";
        }
        else {
            qDebug() << "Couldn't open archive file" << archive_fullname;
        }

        // Read game from the file
        Archiver archiver;
        endnum end_type;
        vector<halfmove> history;
        vector<bitmove> bistory;
        int error = archiver.readGame(end_type, bistory, history, archive_fullname);
        if (!error) {
            startGame(historical);
            for (size_t i = 0, size = history.size(); i < size; ++i)
                history_area->writeStory(i + 1, history[i]);
            board->setEndType(end_type);
            board->setBistory(bistory);
            board->setHistory(history);
        }
        else {
            showBox("Oops",
                "Something went wrong. Error code: " + QString::number(error),
                QMessageBox::Warning);
        }
    }

    void MainWindow::on_actionTraining_triggered()
    {
        startGame(training);
        FenDialog dialog(this);
        connect(&dialog, &FenDialog::newFen, board, &Board::setTilesSlot);
        dialog.exec();
        switchGlow();
    }

    void MainWindow::on_copy_fen_button_clicked()
    {
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(board->getFen());
    }

#ifdef MMDTEST
    void MainWindow::on_test_button_clicked() {
    
        // cout << endl << "Current position:" << board->toFen() << endl;
        VirtualBoard vb(board);
        //vb.turn = board->turn;
        //vb.history = board->history;
        //vb.importTiles(board->tiles);
        //for (int i = 0; i < 6; ++i) {
        //    vb.valid->has_moved[i] = board->valid->has_moved[i];
        //}
    
        int i = 0, depth = ui->message_edit->toPlainText().toInt(); /*5 - board->History().size()*/
        if (depth < 0) {
            cout << endl << "Depth is incorrect" << endl;
            return;
        }
        //auto moves_count = vb.valid->countMovesTest(depth, i);
        auto moves_count = vb.valid->VirtualValidator::countMovesTest(depth, i);
        cout << endl << "Depth: " << depth << ", counted moves : " << moves_count << endl;
    }
#endif  // MMDTEST
}  // namespace mmd
