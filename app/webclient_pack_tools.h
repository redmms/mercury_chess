void WebClient::writePack(package_ty data) {
    send_stream << quint8(data);
}

void WebClient::writePack(quint8 data) {
    send_stream << data;
}

void WebClient::writePack(bool data) {
    send_stream << data;
}

void WebClient::writePack(const QByteArray& data) {
    send_stream.writeBytes(data.data(), data.size());
}

void WebClient::writePack(const QString& data) {
    auto copy = data.toLatin1();
    auto copy_size = data.size();
    send_stream.writeBytes(data.toLatin1(), data.size()); // FIX: or data.ToLatin1().size()?
}

void WebClient::writePack(const QPixmap& data) {
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    data.save(&buffer, "PNG");
    writePack(arr);
}

void WebClient::readPack(package_ty &data)
{
    quint8 byte;
    read_stream >> byte;
    if (byte < package_ty::registration || byte > package_ty::already_registered)
        qDebug() << "Error: package_ty from read_package is out of range";
    else
        data = package_ty(byte);
}

void WebClient::readPack(QByteArray& data) {
    uint len;
    read_stream >> len;
    QByteArray buffer(len, Qt::Uninitialized);
    read_stream.readRawData(buffer.data(), len);
    data = QByteArray(buffer);
}

void WebClient::readPack(QString& data) {
    uint len;
    char* buffer;
    read_stream.readBytes(buffer, len);
    data = QString(buffer);
}

void WebClient::readPack(QPixmap& data)
{
    QByteArray arr;
    readPack(arr);
    if (!data.loadFromData(arr, "PNG"))
        qDebug() << "Error: couldn't load QPixmap from QByteArray in read_package";
}

void WebClient::readPack(bool& data) {
    read_stream >> data;
}

void WebClient::readPack(quint8& data) {
    read_stream >> data;
}
