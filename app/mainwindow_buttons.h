
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
    user_pic = QPixmap(avatar_address).scaled(user_size);
    ui->user_avatar->setPixmap(user_pic);
    ui->profile_avatar->setPixmap(user_pic);
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
        QMessageBox msg_box;
        msg_box.setWindowTitle("So huge!");
        msg_box.setText("This nickname is too long. Maximum length is " +
            QString::number(max_nick));
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
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
    if (last_tab == ui->pre_tab) {
        net->sendToServer(package_ty::registration);
        openTab(ui->friend_connect_tab);
    }
    else
        openTab(last_tab);
}

void MainWindow::on_actionWith_friend_triggered()
{
    if (game_active)
        openStopGameDialog(); //FIX: need to be added to all play action slots in the future, when actions'll be added
    else
        openTab(ui->friend_connect_tab);
}

void MainWindow::on_login_button_clicked()
{
    openTab(ui->login_tab);
    registering = false;
}

void MainWindow::on_registrate_button_clicked()
{
    openTab(ui->login_tab);
    registering = true;
}

void MainWindow::on_end_login_button_clicked()
{
    QString entered_username = ui->login_edit->text();
    if (entered_username.size() <= max_nick) {
        settings.setValue("user_name", entered_username);
        ui->profile_name->setText(entered_username);
        if (registering)
            net->sendToServer(package_ty::registration);
        else
            net->sendToServer(package_ty::login);
    }
    else{
        QMessageBox msg_box;
        msg_box.setWindowTitle("So huge!");
        msg_box.setText("This nickname is too long. Maximum length is " +
            QString::number(max_nick));
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
    }
}

void MainWindow::on_back_from_login_button_clicked()
{
    openTab(ui->pre_tab);
}

void MainWindow::on_guest_button_clicked()
{
    QString random_username = "Lazy" + QString::number(std::rand() % (int)std::pow(10, max_nick - 4));
    settings.setValue("user_name", random_username);
    net->sendToServer(package_ty::registration);
    net->sendToServer(package_ty::login);
    openTab(ui->friend_connect_tab);
}

void MainWindow::on_send_invite_button_clicked() // FIX: here - package_ty::invite, user's name, user's image,
{
    int chosen_time = settings.value("time_setup").toInt();
    QString friend_name = ui->friend_name_edit->text();
    QString user_name = settings.value("user_name").toString();
    if (!chosen_time) {
        QMessageBox msg_box;
        msg_box.setWindowTitle("Set up match timer");
        msg_box.setText("You need to choose initial time for chess clock.");
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
    }
    else if (friend_name.isEmpty()) {
        QMessageBox msg_box;
        msg_box.setWindowTitle("Enter friend's name");
        msg_box.setText("You need to choose friend's name.");
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
    }
    else if (friend_name == user_name) {
        QMessageBox msg_box;
        msg_box.setWindowTitle("Too cunning");
        msg_box.setText("You can't send invite to yourself");
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
    }
    else {
        QEventLoop loop; // FIX: should whow "Waiting for friend's respond" message with a rolling widget
        connect(net, &WebClient::endedReadingInvite, &loop, &QEventLoop::quit);
        connect(net, &WebClient::endedReadingInvite, [&](){
            QApplication::restoreOverrideCursor();
        });
        bool match_side = std::rand() % 2;
        settings.setValue("match_side", match_side);
        settings.setValue("opp_name", friend_name);
        net->sendToServer(package_ty::invite);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        loop.exec();
    }
}

void MainWindow::editReturnSlot()
{
    QString message_text = ui->message_edit->toPlainText();
    printMessage(settings.value("user_name").toString(), true, message_text);
    ui->message_edit->clear();
    net->sendToServer(package_ty::chat_message, false, message_text); // FIX: works even after the end of the game
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui->message_edit && ui->message_edit->hasFocus() && event->type() == QEvent::KeyPress){
        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
        if (key_event && (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter)){
            emit editReturnPressed();
        }
        else{
            return QMainWindow::eventFilter(object, event);
        }
    }
    else{
        return QMainWindow::eventFilter(object, event);
    }
}

