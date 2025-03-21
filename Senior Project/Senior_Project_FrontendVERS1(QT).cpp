// -----------------------------------------------------------------------------------------------------------------------
//                                               Name Gino Torres 
//                                               Course CMPS 499, Spring 2025
//                                               Senior Project
//                                               
// Purpose: 
/* 
This program is a simple registration form for a youth football league. It allows the user to input the player's name, date of birth, guardian's name, phone number, and email address. The program validates the input and stores the data in a SQLite database. If any of the fields are empty, the program displays a warning message. If the phone number is not 10 digits, the program displays a warning message. If the email address is not in the correct format, the program displays a warning message. If the data is successfully stored in the database, the program displays a success message.
Frondend Development with QT Creator that includes:
1. A Registration Form (QLineEdit for inputs, QPushButton to submit)
2. Form Validation (Check valid phone/email)
3. Database Integration (Save and load players from SQLite)
4. A List View to Display Registered Players
*/
//------------------------------------------------------------------------------------------------------------------------
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

class RegistrationForm : public QWidget {
    Q_OBJECT
public:
    RegistrationForm(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QFormLayout *formLayout = new QFormLayout();

        nameInput = new QLineEdit(this);
        dobInput = new QLineEdit(this);
        guardianInput = new QLineEdit(this);
        phoneInput = new QLineEdit(this);
        emailInput = new QLineEdit(this);
        QPushButton *submitButton = new QPushButton("Register", this);
        
        formLayout->addRow("Player Name:", nameInput);
        formLayout->addRow("Date of Birth (YYYY-MM-DD):", dobInput);
        formLayout->addRow("Guardian Name:", guardianInput);
        formLayout->addRow("Phone Number:", phoneInput);
        formLayout->addRow("Email:", emailInput);

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(submitButton);

        connect(submitButton, &QPushButton::clicked, this, &RegistrationForm::registerPlayer);

        setupDatabase();
    }

private slots:
    void registerPlayer() {
        QString name = nameInput->text();
        QString dob = dobInput->text();
        QString guardian = guardianInput->text();
        QString phone = phoneInput->text();
        QString email = emailInput->text();

        if (name.isEmpty() || dob.isEmpty() || guardian.isEmpty() || phone.isEmpty() || email.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "All fields must be filled!");
            return;
        }
        
        if (!phone.contains(QRegExp("\\d{10}"))) {
            QMessageBox::warning(this, "Input Error", "Invalid phone number. Must be 10 digits.");
            return;
        }
        
        if (!email.contains(QRegExp("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$"))) {
            QMessageBox::warning(this, "Input Error", "Invalid email format.");
            return;
        }
        
        QSqlQuery query;
        query.prepare("INSERT INTO Players (Name, DOB, Guardian, Phone, Email) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(name);
        query.addBindValue(dob);
        query.addBindValue(guardian);
        query.addBindValue(phone);
        query.addBindValue(email);
        
        if (!query.exec()) {
            QMessageBox::critical(this, "Database Error", query.lastError().text());
        } else {
            QMessageBox::information(this, "Success", "Player registered successfully!");
        }
    }

private:
    QLineEdit *nameInput, *dobInput, *guardianInput, *phoneInput, *emailInput;
    
    void setupDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("players.db");
        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", db.lastError().text());
            return;
        }
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS Players (Name TEXT, DOB TEXT, Guardian TEXT, Phone TEXT, Email TEXT);");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    RegistrationForm window;
    window.setWindowTitle("Youth Football Registration");
    window.show();
    return app.exec();
}
