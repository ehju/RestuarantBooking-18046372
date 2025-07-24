#include <iostream>
#include "mail_sender.cpp"

class TestableMailSender : public MailSender {
public:
	void sendMail(Schedule* schedule) override {
		//send message to PhoneNumber
		//std::cout << "�׽�Ʈ�� MailSender class �� send�޼��� �����\n";
		countSendMailMethodIsCalled++;
	}

	int getCountSendMailMethodCalled() {
		return countSendMailMethodIsCalled;
	}
private:
	int countSendMailMethodIsCalled= 0;
};