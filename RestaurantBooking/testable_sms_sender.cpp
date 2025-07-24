#include <iostream>
#include "sms_sender.cpp"

class TestableSmsSender : public SmsSender {
public:
	void send(Schedule* schedule) override{
		//send message to PhoneNumber
		std::cout << "�׽�Ʈ�� SmsSender class �� send�޼��� �����\n";
		sendMethodIsCalled = true;
	}

	bool isSendMethodIsCalled() {
		return sendMethodIsCalled;
	}
private:
	bool sendMethodIsCalled;
};