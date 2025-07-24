#include <iostream>
#include "sms_sender.cpp"

class TestableSmsSender : public SmsSender {
public:
	void send(Schedule* schedule) override{
		//send message to PhoneNumber
		std::cout << "테스트용 SmsSender class 의 send메서드 실행됨\n";
		sendMethodIsCalled = true;
	}

	bool isSendMethodIsCalled() {
		return sendMethodIsCalled;
	}
private:
	bool sendMethodIsCalled;
};