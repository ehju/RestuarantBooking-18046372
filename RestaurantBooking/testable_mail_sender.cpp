#include <iostream>
#include "mail_sender.cpp"

class TestableMailSender : public MailSender {
public:
	void sendMail(Schedule* schedule) override {
		//send message to PhoneNumber
		//std::cout << "테스트용 MailSender class 의 send메서드 실행됨\n";
		countSendMailMethodIsCalled++;
	}

	int getCountSendMailMethodCalled() {
		return countSendMailMethodIsCalled;
	}
private:
	int countSendMailMethodIsCalled= 0;
};