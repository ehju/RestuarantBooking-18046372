#include "gmock/gmock.h"
#include "booking_scheduler.cpp"
#include "testable_sms_sender.cpp"
#include "testable_mail_sender.cpp"
#include "sunday_booking_scheduler.cpp"
#include "monday_booking_scheduler.cpp"
using namespace testing;

class BookingItem : public Test {
protected:
	void SetUp() override {
		NOT_ON_THE_HOUR = getTime(2021, 3, 26, 9, 5);
		ON_THE_HOUR = getTime(2021, 3, 26, 9, 0);

		bookingScheduler.setSmsSender(&testableSmsSender);
		bookingScheduler.setMailSender(&testableMailSender);
	}
public:
	tm getTime(int year, int mon, int day, int hour, int min) {
		tm result = { 0, min, hour, day, mon - 1, year - 1900, 0, 0, -1 };
		mktime(&result);
		return result;
	}
	tm plusHour(tm base, int hour) {
		base.tm_hour += hour;
		mktime(&base);
		return base;
	}
	tm NOT_ON_THE_HOUR;
	tm ON_THE_HOUR;
	Customer CUSTOMER{ "Fake name", "010-1234-5678" };
	Customer CUSTOMER_WITH_MAIL { "Fake name", "010-1234-5678", "test@test.com"};
	const int UNDER_CAPACITY = 1;
	const int CAPACITY_PER_HOUR = 3;

	BookingScheduler bookingScheduler{ CAPACITY_PER_HOUR };
	TestableSmsSender testableSmsSender;
	TestableMailSender testableMailSender;

};

TEST_F(BookingItem, 예약은정시에만가능하다정시가아닌경우예약불가) {
	//arrange
	Schedule* schedule = new Schedule{ NOT_ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };
	

	//act
	EXPECT_THROW({
			bookingScheduler.addSchedule(schedule);
		}, std::runtime_error);
}

TEST_F(BookingItem, 예약은정시에만가능하다정시인경우예약가능) {
	//arrange
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };
	//act
	bookingScheduler.addSchedule(schedule);
	//assert
	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));
}

TEST_F(BookingItem, 시간대별인원제한이있다같은시간대에Capacity초과할경우예외발생) {
	//arrange
	//precondition : full booking
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER }; 
	bookingScheduler.addSchedule(schedule);
	//act
	try {
		Schedule* newSchedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };
		bookingScheduler.addSchedule(newSchedule); // exception
		FAIL(); // not arrivable
	}
	catch (std::runtime_error& e){
		//assert
		EXPECT_EQ(string{ e.what() }, string{"Number of people is over restaurant capacity per hour"});
	}
}

TEST_F(BookingItem, 시간대별인원제한이있다같은시간대가다르면Capacity차있어도스케쥴추가성공) {
	//arrange
    //precondition : full booking
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER };
	bookingScheduler.addSchedule(schedule);
	//act
	tm differentHour = plusHour(ON_THE_HOUR,1);
	mktime(&differentHour);
	Schedule* newSchedule = new Schedule{ differentHour, UNDER_CAPACITY, CUSTOMER };
	bookingScheduler.addSchedule(newSchedule); // exception
	
	EXPECT_EQ(true, bookingScheduler.hasSchedule(newSchedule));
}

TEST_F(BookingItem, 예약완료시SMS는무조건발송) {
	
	//arrange
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER };
	
	//act
	bookingScheduler.addSchedule(schedule); 

	EXPECT_EQ(true, testableSmsSender.isSendMethodIsCalled());
}

TEST_F(BookingItem, 이메일이없는경우에는이메일미발송) {
	//arrange
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER };

	//act
	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(0, testableMailSender.getCountSendMailMethodCalled());
}

TEST_F(BookingItem, 이메일이있는경우에는이메일발송) {
	//arrange
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER_WITH_MAIL };

	//act
	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(1, testableMailSender.getCountSendMailMethodCalled());
}

TEST_F(BookingItem, 현재날짜가일요일인경우예약불가예외처리) {
	//arrange
	BookingScheduler* bookingScheduler = new SundayBookingScheduler{ CAPACITY_PER_HOUR };
	
	//act
	try {
		Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };
		bookingScheduler->addSchedule(schedule); // exception
		FAIL(); // not arrivable
	}
	catch (std::runtime_error& e) {
		//assert
		EXPECT_EQ(string{ e.what() }, string{ "Booking system is not available on sunday"});
	}
}

TEST_F(BookingItem, 현재날짜가일요일이아닌경우예약가능) {
	//arrange
	BookingScheduler* bookingScheduler = new MondayBookingScheduler{ CAPACITY_PER_HOUR };

	//act
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };
	bookingScheduler->addSchedule(schedule); // exception

	EXPECT_EQ(true, bookingScheduler->hasSchedule(schedule));
}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}