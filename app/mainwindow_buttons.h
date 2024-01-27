
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
    if (new_name.size() <= max_nick) {
        settings.setValue("user_name", new_name);
        ui->user_name->setText(new_name);
        ui->profile_name->setText(new_name);
    }
    else {
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
    if (game_active && game_regime == "friend_online")
        openStopGameDialog(); //FIX: need to be added to all play action slots in the future, when actions'll be added
    else
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
         settings.setValue("user_name", entered_username);
         QCryptographicHash hasher(QCryptographicHash::Sha512);
         hasher.addData(ui->login_password->text().toLocal8Bit());
         QByteArray hashed_password = hasher.result();
         settings.setValue("user_pass", hashed_password);
         ui->profile_name->setText(entered_username);
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
    settings.setValue("user_name", random_username);
    net->sendToServer(package_ty::registration);
    net->sendToServer(package_ty::login);
}

void MainWindow::on_send_invite_button_clicked() // FIX: here - package_ty::invite, user's name, user's image,
{
//    if (waiting_for_invite_respond){
//        qDebug() << curTime() << "Tried to send match invite several times";
//        return;
//    }
    int chosen_time = settings.value("time_setup").toInt();
    QString friend_name = ui->friend_name_edit->text();
    QString user_name = settings.value("user_name").toString();
    if (!chosen_time) {
        showBox("Set up match timer",
                "You need to choose initial time for chess clock.",
                QMessageBox::Warning);
    }
    else if (friend_name.isEmpty()) {
        showBox("Enter friend's name",
                "You need to choose friend's name.",
                QMessageBox::Warning);
    }
    else if (friend_name == user_name) {
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
        settings.setValue("opp_name", friend_name);
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
    QString game_regime = settings.value("game_regime").toString();
    if (game_active && game_regime == "friend_online")
        openStopGameDialog();
    else{
        settings.setValue("game_regime", "friend_offline");
        startGame();
    }
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

