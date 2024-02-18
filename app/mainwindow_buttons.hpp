
void MainWindow::on_draw_button_clicked()
{
    net->sendToServer(package_ty::draw_suggestion);
}

void MainWindow::on_resign_button_clicked()
{
    net->sendToServer(package_ty::resignation);
    endSlot(endnum::user_resignation);
}

void MainWindow::on_actionProfile_triggered()
{
    openTab(ui->settings_tab);
}

void MainWindow::on_change_photo_button_clicked()
{
    // open dialog window to choose a photo
    QString avatar_address = QFileDialog::getOpenFileName(this, "Open File",
        QString("Choose a photo for avatar. Avatar picture will be scaled to 100x100 pixel image."),
        tr("Images (*.png *.jpg *.jpeg *.pgm)"));
    QSize user_size = ui->user_avatar->size();
    if (!avatar_address.isEmpty()){
        user_pic = QPixmap(avatar_address).scaled(user_size);
        ui->profile_avatar->setPixmap(user_pic);
        if (game_active)
            ui->user_avatar->setPixmap(user_pic);
    }
}

void MainWindow::on_change_name_button_clicked()
{
    QString new_name = ui->name_edit->text();
    ui->name_edit->clear();
    if (net) {
        try {
            net->sendToServer(package_ty::new_name, 0, new_name);
        }
        catch (const exception& e) {
            showBox("Can't change online nickname",
                "You are offline, only local nickname has been changed. "
                "To change online nickname you will need to reenter account. "
                "Contact me by mmd18cury@yandex.ru to start the server");
        }
    }
    int err = changeLocalName(new_name);
    if (err == 1) {
        showBox("Embarrasing!",
            "This nickname is too small. It can't be empty.",
            QMessageBox::Warning);
    }
    else if (err == 2) {
        showBox("So huge!",
            "This nickname is too long. Maximum length is " + QString::number(max_nick),
            QMessageBox::Warning);
    }
    //    QMessageBox msgBox;
    //    msgBox.setWindowTitle("Notification");
    //    msgBox.setText("Nickname has been changed");
    //    msgBox.setIcon(QMessageBox::Information);
    //    msgBox.addButton("Ok", QMessageBox::AcceptRole);
    ////    connect(&msgBox, &QMessageBox::accepted, &msgBox, &QMessageBox::close);
    //    msgBox.exec();
}

void MainWindow::on_back_from_settings_clicked()
{
    openTab(last_tab.data());
}

void MainWindow::on_actionWith_friend_triggered()
{
    QString game_regime = settings.value("game_regime").toString();
    openTab(ui->friend_connect_tab);
}

void MainWindow::on_login_button_clicked()
{
    openTab(ui->login_tab);
    login_regime = 1;
}

void MainWindow::on_registrate_button_clicked()
{
    openTab(ui->login_tab);
    login_regime = 2;
}

void MainWindow::on_end_login_button_clicked()
{
    QString entered_username = ui->login_edit->text();
    if (ui->login_password->text().isEmpty()){
        showBox("Too small for the party!",
                "You need to enter a login",
                QMessageBox::Warning);
    }
    else if (entered_username.isEmpty()){
        showBox("Too small for the party!",
                "You need to enter a password",
                QMessageBox::Warning);
    }
    else if (entered_username.size() > max_nick){
        showBox("So huge!",
                "This nickname is too long. Maximum length is " + QString::number(max_nick),
                QMessageBox::Warning);
    }
    else if (ui->login_password->text().size() < 6){
        showBox("Too small for the party!",
                "The password should be at least 7 symbols long",
                QMessageBox::Warning);
    }
    else {
         changeLocalName(entered_username);
         QCryptographicHash hasher(QCryptographicHash::Sha512);
         hasher.addData(ui->login_password->text().toLocal8Bit());
         QByteArray hashed_password = hasher.result();
         settings.setValue("user_pass", hashed_password);
         if (login_regime == 2)
            net->sendToServer(package_ty::registration);
         else if (login_regime == 1)
            net->sendToServer(package_ty::login);
    }
}

void MainWindow::on_back_from_login_button_clicked()
{
    openTab(ui->pre_tab);
}

void MainWindow::on_guest_button_clicked()
{
    login_regime = 3;
    QString random_username = "Lazy" + QString::number(std::rand() % (int)std::pow(10, max_nick - 4));
    changeLocalName(random_username);
    net->sendToServer(package_ty::registration);
    net->sendToServer(package_ty::login);
}

void MainWindow::on_send_invite_button_clicked() // FIX: here - package_ty::invite, user's name, user's image,
{
//    if (waiting_for_invite_respond){
//        qDebug() << "Tried to send match invite several times";
//        return;
//    }
    int chosen_time = settings.value("time_setup").toInt();
    QString opp_name = ui->friend_name_edit->text();
    QString user_name = settings.value("user_name").toString();
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
        //QEventLoop loop; // FIX: should whow "Waiting for friend's respond" message with a rolling widget
//        connect(net.data(), &WebClient::endedReadingInvite, [&](){
//            QApplication::restoreOverrideCursor();
//            waiting_for_invite_respond = false;
//            loop.quit();
//        });
        bool match_side = std::rand() % 2;
        settings.setValue("match_side", match_side);
        settings.setValue("opp_name", opp_name);
        net->sendToServer(package_ty::invite);
       // QApplication::setOverrideCursor(Qt::WaitCursor);
        waiting_for_invite_respond = true;
        //loop.exec();
    }
}

void MainWindow::editReturnSlot()
{
    QString message_text = ui->message_edit->toPlainText();
    printMessage(settings.value("user_name").toString(), true, message_text);
    ui->message_edit->clear();
    if (settings.value("game_regime").toString() == "friend_online")
        net->sendToServer(package_ty::chat_message, false, message_text); // FIX: works even after the end of the game
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui->message_edit && ui->message_edit->hasFocus() && event->type() == QEvent::KeyPress){
        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
        if (key_event && (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter)){
            emit editReturnPressed();
            return true;
        }
        else{
            return QMainWindow::eventFilter(object, event);
        }
    }
    else{
        return QMainWindow::eventFilter(object, event);
    }
}

void MainWindow::on_actionToggle_fullscreen_triggered()
{
    static bool fullscreen = false;
    if (fullscreen){
        showMaximized();
        if (game_active)
            statusBar()->show();
    }
    else{
        showFullScreen();
        if (game_active)
            statusBar()->hide();
    }
    fullscreen = !fullscreen;
}

void MainWindow::on_actionWith_friend_offline_triggered()
{
    OfflineDialog dialog(this, default_pic);
    connect(&dialog, &OfflineDialog::newOppName, [&](QString name) {
        settings.setValue("opp_name", name);
    });
    connect(&dialog, &OfflineDialog::newOppPic, [&](QPixmap pic) {
        opp_pic = pic;
    });
    auto res = dialog.exec();
    if (res == QDialog::Rejected){
        opp_pic = default_pic;
        settings.setValue("opp_name", "Friend");
    }
    //QString game_regime = settings.value("game_regime").toString();
    //settings.setValue("game_regime", "friend_offline");
    startGame("friend_offline");
}

void MainWindow::on_actionEnter_triggered()
{
    openTab(ui->pre_tab);
}

void MainWindow::on_change_ip_button_clicked()
{
    QString new_address = ui->ip_edit->text();
    int new_port = ui->port_edit->text().toInt();
    settings.setValue("ip_address", new_address);
    settings.setValue("port_address", new_port);
    net->connectNewHost();
}

void MainWindow::on_restore_default_button_clicked()
{
    settings.setValue("ip_address", default_address);
    settings.setValue("port_address", default_port);
    net->connectNewHost();
    ui->ip_edit->clear();
    ui->port_edit->clear();
}

void MainWindow::on_offline_stop_button_clicked()
{
    endSlot(endnum::interrupt);
}

void MainWindow::on_offline_back_button_clicked()
{
    openTab(last_tab);
}

void MainWindow::on_history_next_button_clicked()
{
    openInDevDialog();
    //auto& history = board->history;
    //halfmove move = history[current_move];
    //Tile* from = move.move.first.tile;
    //Tile* to = move.move.second.tile;
    //board->halfMove(from, to);
    //current_move++;
}

void MainWindow::on_history_previous_button_clicked()
{
    openInDevDialog();
    //auto& history = *board->history;
    //halfmove move = history[current_move];
    //board.revertMove(move);
    //current_move--;
}

void MainWindow::on_actionAbout_triggered()
{
    QString title = "About Mercury Chess";
    QString description = "I am Max Cury (MMD18) and this is my pet project written in C++ programming language with Qt framework.<br><br>"
        "It may seem that the main feature of the program is a chess game with a beautiful style, but it is absolutely not.<br><br>The main feature is its built-in archiver "
        "with its own chess data format using a sophisticated algorithm to compress the data, so that it takes even less than 1 byte of space for every halfmove and uses its own "
        "bitstream allowing it to operate individual bits in std::cout style (available on <a href = 'https://github.com/redmms/finestream.git'>github</a>).<br><br>I also invented several algorithms for describing chess board positions "
        "(like FEN but with a different structure and encoded in bits) and will code them if I find some "
        "interest of programmers or chess community. "
        "<br><br>"
        "To contact me, use this email: mmd18cury@yandex.ru";
    QMessageBox msg_box;
    msg_box.setWindowTitle(title);
    msg_box.setTextFormat(Qt::RichText);   //this is what makes the links clickable
    msg_box.setText(description);
    msg_box.exec();
    //QString description = "a" + QString(std::endl) + "<a href = 'https://github.com/redmms/finestream.git'>github< / a>";
    //QMessageBox::about(0, title, description);
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(0, "About Qt");
}

void MainWindow::on_actionWith_friend_2_triggered()
{
    openInDevDialog();
}


void MainWindow::on_actionTraining_triggered()
{
    openInDevDialog();
}


void MainWindow::on_actionRandomly_triggered()
{
    openInDevDialog();
}


void MainWindow::on_actionRules_triggered()
{
    openInDevDialog();
}


void MainWindow::on_actionSend_suggestion_triggered()
{
    openInDevDialog();
}


void MainWindow::on_actionReport_a_bug_triggered()
{
    openInDevDialog();
}
