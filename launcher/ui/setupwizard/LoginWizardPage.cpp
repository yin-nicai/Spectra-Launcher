#include "LoginWizardPage.h"
#include "minecraft/auth/AccountList.h"
#include "minecraft/auth/MinecraftAccount.h"
#include "ui/dialogs/ChooseOfflineNameDialog.h"
#include "ui/dialogs/MSALoginDialog.h"
#include "ui_LoginWizardPage.h"

#include "Application.h"

LoginWizardPage::LoginWizardPage(QWidget* parent) : BaseWizardPage(parent), ui(new Ui::LoginWizardPage)
{
    ui->setupUi(this);
    ui->pushButton->setVisible(APPLICATION->capabilities() & Application::SupportsMSA);
}

LoginWizardPage::~LoginWizardPage()
{
    delete ui;
}

void LoginWizardPage::initializePage() {}

bool LoginWizardPage::validatePage()
{
    return true;
}

void LoginWizardPage::retranslate()
{
    ui->retranslateUi(this);
    ui->pushButton->setVisible(APPLICATION->capabilities() & Application::SupportsMSA);
}

void LoginWizardPage::completeLogin(const MinecraftAccountPtr& account)
{
    if (!account) {
        return;
    }

    auto* accounts = APPLICATION->accounts();
    const bool setAsDefault = accounts->defaultAccount() == nullptr;
    accounts->addAccount(account);
    if (setAsDefault) {
        accounts->setDefaultAccount(account);
    }

    if (wizard()->currentId() == wizard()->pageIds().last()) {
        wizard()->accept();
    } else {
        wizard()->next();
    }
}

void LoginWizardPage::on_pushButton_clicked()
{
    wizard()->hide();
    auto account = MSALoginDialog::newAccount(nullptr);
    wizard()->show();
    completeLogin(account);
}

void LoginWizardPage::on_offlineButton_clicked()
{
    wizard()->hide();
    ChooseOfflineNameDialog dialog(tr("Enter a username for your offline account. It may contain 3-16 letters, numbers, or underscores. "
                                      "Offline accounts can play locally and on offline-mode servers. Please support genuine Minecraft."),
                                   wizard());
    const auto result = dialog.exec();
    wizard()->show();
    if (result != QDialog::Accepted) {
        return;
    }

    if (const auto account = MinecraftAccount::createOffline(dialog.getUsername())) {
        account->login()->start();
        completeLogin(account);
    }
}
