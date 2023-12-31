#include "sqlliteDB.h"



SqlLiteDb::SqlLiteDb(QString pathFile)
{
    setPath(pathFile);
}

// Проверка работы БД
bool SqlLiteDb::testRunBd()
{
    return runBd;
}

// Метод для установки пути файла к БД.
void SqlLiteDb::setPath(QString pathFile)
{
    this->pathFile = pathFile;
    sdb = QSqlDatabase::addDatabase("QSQLITE");
    QFileInfo check_file(this->pathFile);
    runBd = check_file.exists(); // переменная для проверки запустилась ли БД
    sdb.setDatabaseName(this->pathFile);
    sdb.open();
    if(!runBd){
       runQuery(CREATE_TABLE_FILES, "create");
       runQuery(CREATE_TABLE_BLOCK, "create");
       runQuery(CREATE_TABLE_BOARD, "create");
       runQuery(CREATE_TABLE_PORT, "create");
       cout << "Create DB - " << qPrintable(check_file.absoluteFilePath()) << endl;
       runBd = true;
    }

}

// Метод для select запроса
QVector<QMap<QString, QString>> SqlLiteDb::runQuerySelect(QString query)
{
    QVector<QMap<QString, QString>> result;
    QSqlQuery querySelect;

    if (!querySelect.exec(query)) {
        cout << "Bad select!" << endl;
        return result;
    }
    while (querySelect.next()) {
        QMap<QString, QString> row;
        QSqlRecord localRecord = querySelect.record();
        for (int var = 0; var < localRecord.count(); ++var) {
            QSqlField field = localRecord.field(var);
            row[field.name()] = field.value().toString();
        }
        result.push_back(row);
    }
    return result;
}

// Метод для insert запроса с помощью готового запроса
bool SqlLiteDb::runQueryInsert(QString query)
{
    QSqlQuery insertQuery;
    return insertQuery.exec(query);
}

// Метод для insert запроса с помощью названия таблицы и словаря данных для вставки
bool SqlLiteDb::runQueryInsert(QString tableName, QMap<QString, QString> data)
{
    QSqlQuery insertQuery;
    QString query = createQuery(tableName, data);
    return insertQuery.exec(query);
}

// Метод для delete запроса
bool SqlLiteDb::runQueryDelete(QString query)
{
    QSqlQuery deleteQuery;
    deleteQuery.exec("PRAGMA foreign_keys = ON"); // запрос для включения каскадного удаления данных
    return deleteQuery.exec(query);
}

// Метод для update запроса
bool SqlLiteDb::runQueryUpdate(QString query)
{
    QSqlQuery updateQuery;
    return updateQuery.exec(query);
}

// Метод для create запроса
bool SqlLiteDb::runQueryCreate(QString query)
{
    QSqlQuery createQuery;
    return createQuery.exec(query);
}

// Универсальный метод для разных типов запросов
QVector<QMap<QString, QString>> SqlLiteDb::runQuery(QString query, QString typeQuery)
{
    QVector<QMap<QString, QString>> result;
    QMap<QString, QString> error;
    if (typeQuery == "insert"){
        if(!runQueryInsert(query)) {
            cout << "Bad Insert!" << endl;
            error["error"] = "Bad Insert!";
            result.push_back(error);
        }
    }
    else if (typeQuery == "update") {
        if(!runQueryUpdate(query)) {
            cout << "Bad Update!" << endl;
            error["error"] = "Bad Update!";
            result.push_back(error);
        }

    }
    else if (typeQuery == "delete") {
        if(!runQueryDelete(query)) {
            cout << "Bad Delete!" << endl;
            error["error"] = "Bad Delete!";
            result.push_back(error);
        }
    }
    else if (typeQuery == "select") {
        result = runQuerySelect(query);
    }
    else if(typeQuery == "create") {
        if(!runQueryCreate(query)) {
            cout << "Bad Create!" << endl;
            error["error"] = "Bad Create!";
            result.push_back(error);
        }
    }
    return result;
}

// Метод для создания insert-запроса с помощью названия таблицы и словаря данных.
QString SqlLiteDb::createQuery(QString tableName, QMap<QString, QString> data)
{
    QString queryBegin = BEGIN_INSERT_QUERY; // шаблон начала запроса
    QString queryEnd = VALUES_INSERT_QUERY; // шаблон середины запроса
    queryBegin = queryBegin.arg(tableName); // добавление названия таблицы
    for(auto e : data.keys())
    {
      queryBegin += QString("%1,").arg(e);
      queryEnd += QString("'%1',").arg(data.value(e));
    }
    // удаление последних запятых в получившихся шаблонах
    int pos = queryBegin.lastIndexOf(QChar(','));
    queryBegin = queryBegin.left(pos);
    pos = queryEnd.lastIndexOf(QChar(','));
    queryEnd = queryEnd.left(pos);
    return queryBegin + queryEnd + QString(END_INSERT_QUERY);
}
