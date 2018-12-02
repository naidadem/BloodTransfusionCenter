#include <iostream>
#include <regex>
#include <thread>
#include <mutex>
#include <vector>
#include <ctname>
using namespace std;
const char *crt = "\n-------------------------------------------\n";
enum BloodGroupMark { O, A, B, AB };
mutex MyMutex;

char* BloodGroupMarkString[] = { "O", "A", "B", "AB" };
class DateD {
	int _day, _month, _year;
public:
	DateD(int day, int month, int year) :
		_day(day), _month(month), _year(year) {}
	string ToString(char separator = '.') const
	{
		// 21.03.2006
		string date;
		date += to_string(_day);
		date += separator;
		date += to_string(_month);
		date += separator;
		date += to_string(_year);
		return date;
	}
	bool DifferenceDatesThreeMonths(DateD & dat)//better solution so that dates are converted to days, and then calculate the difference
	{
		int monthsDifference = abs(_month - dat._month);
		if (_year>dat._year)//if year of request has araised before year of donation input is incorrect
		{
			cout << "Error!Year isn't valid!"; return false;
		}
		if (dat._year == _year)
		{
			if (_month > dat._month)//if month of request has araised before month of donation input is incorrect
			{
				cout << "Error!Month isn't valid!"; return false;
			}
			else if (monthsDifference < 3)//if it's the same year but difference between months is lower than 3 return false
			{
				return false;
			}
		}
		else if (_year == dat._year - 1)//if year of donation has been over in comparation to request
		{
			if (_month > 9 && dat._month > 3)//if months aren't in between of 10th and 2nd month
				return true;
			else if (monthsDifference<9)// For a transition between 2 years the difference will be more than three months if it is
				//For example : 10 - 2 = 8 (11, 12, 1-> 3); 12 - 3 = 9 (1, 2-> 2)
			{
				return true;
			}
			else return false;
		}
		else //if many year have been over between donation and request three months have been over as well
		{
			return true;
		}
	}
};
template <class T1, class T2, int max>
class Collection {
	T1 _elements1[max];
	T2 _elements2[max];
	int _currentlyElements;
	int _keyElement; // key element, implies T1 as key element
					 // and then the value is 1, in case it is T2 the value will be 2
public:
	Collection(int keyElement = 1) {
		_currentlyElements = 0;
		_keyElement = keyElement;
	}
	bool AddElement(T1 ele1, T2 ele2) {
		if (_currentlyElements == max)
			return false;
		_elements1[_currentlyElements] = ele1;
		_elements2[_currentlyElements] = ele2;
		_currentlyElements++;
		return true;
	}
	void SetElement2(int location, T2 ele2) {
		_elements2[location] = ele2;
	}
	void SetKeyElement(int keyElement) {
		if (keyElement < 1 || keyElement>2)
			throw exception("Unallowed value!");
		_keyElement = keyElement;
	}
	void Change(int i, int j)
	{
		T1 temp1 = _elements1[i];
		T2 temp2 = _elements2[i];
		_elements1[i] = _elements1[j];
		_elements2[i] = _elements2[j];
		_elements1[j] = temp1;
		_elements2[j] = temp2;
	}
	T1 * GetElements1() { return _elements1; }
	T2 * GetElements2() { return _elements2; }

	void Sort(string filter)
	{
		if (filter == "ASC")
		{
			for (size_t i = 0; i < _currentlyElements - 1; i++)
			{
				if (_elements1[i]>_elements1[i + 1])
				{
					for (size_t j = i; j < _currentlyElements - 1; j++)
					{
						if (_elements1[j] > _elements1[j + 1])
						{
							Change(j, j + 1);
						}
					}
				}
			}
		}
		// 0123456789
		// 9876543210

		else if (filter == "DESC")
		{
			for (size_t i = 0; i < _currentlyElements - 1; i++)
			{
				if (_elements1[i + 1]>_elements1[i])
				{
					Change(i, i + 1);
					for (size_t j = i + 1; j > 0; j--)
					{
						if (_elements1[j]>_elements1[j - 1])
						{
							Change(j, j - 1);
						}
					}
				}
			}
		}
		else cout << "Wrong sorting format!" << endl;
	}
	int GetCurrently() const { return _currentlyElements; }
	int GetMax() const { return max; }
	T1 GetElement1(int location) const {
		if (location < 0 || location >= _currentlyElements)
			throw exception("Location doesn't exist!");
		return _elements1[location];
	}
	T2 GetElement2(int location) const {
		if (location < 0 || location >= _currentlyElements)
			throw exception("Location doesn't exist!");
		return _elements2[location];
	}
	friend ostream &operator<<(ostream &COUT, const Collection &obj) {
		for (size_t i = 0; i < obj.GetCurrently(); i++)
			COUT << obj.GetElement1(i) << " " << obj.GetElement2(i) << endl;
		return COUT;
	}
};
class BloodGroup {
	/*there are 4 blood groups that are listed in the BloodGroupMark enumeration, at which
		each of these can have a positive(+) and negative(-) Rh factor */
	BloodGroupMark _mark;//eg: AB
	char _rhFactor; // + or -
	/*the above mentioned is extremely important because of the reason
		that there are clear rules regarding blood donation ie. which recipient can receive blood from
		which donor (as shown in the table), and the next two vectors are responsible for keeping
information about it, eg. for the blood group A+ vector donors will keep the values: A+ and AB+
		*/
	vector<BloodGroup> _donors; // blood groups that a given blood group can donate blood
	vector<BloodGroup> _bloodRecipients; // blood groups that a given blood group can receive blood
public:
	BloodGroup(BloodGroupMark mark = O, char rhFactor = ' ') :
		_mark(mark), _rhFactor(rhFactor) {}
	BloodGroup(const BloodGroup &k) :_mark(k._mark), _rhFactor(k._rhFactor), _donors(k._donors), _bloodRecipients(k._bloodRecipients) { ; }
	BloodGroup & operator=(const BloodGroup &k) {
		_mark = (k._mark);
		_rhFactor = (k._rhFactor);
		_donors = (k._donors);
		_bloodRecipients = (k._bloodRecipients);
		return *this;
	}

	void SetDonors(vector<BloodGroup> donors) {
		_donors = donors;
	}
	void SetBloodRecipients(vector<BloodGroup> bloodRecepients) {
		_bloodRecipients = bloodRecepients;
	}
	friend bool operator==(const BloodGroup & k1, const BloodGroup & k2)
	{
		return k1._mark == k2._mark && k2._rhFactor == k1._rhFactor;
	}
	string ToString() const
	{
		string printing;

		printing += crt + string("Blood group ->");
		printing += string(BloodGroupMarkString[_mark]) + _rhFactor;
		printing += string(crt) + "Donors ->";
		for (size_t i = 0; i < _donors.size(); i++)
		{
			printing += string(BloodGroupMarkString[_donors[i]._mark]) + _donors[i]._rhFactor + ',';
		}
		printing += crt;
		for (size_t i = 0; i < _bloodRecipients.size(); i++)
		{
			printing += string(BloodGroupMarkString[_bloodRecipients[i]._mark]) + _bloodRecipients[i]._rhFactor + ',';
		}
		return printing + crt;
	}
};
//Make the class abstract
class Person {
protected:
	char* _name;
	BloodGroup _bloodGroup;
public:
	Person(char* name, BloodGroup bloodGroup) : _bloodGroup(bloodGroup) {
		int size = strlen(name) + 1;
		_name = new char[size];
		strcpy_s(_name, size, name);
	}
	Person(const Person &k) :_bloodGroup(k._bloodGroup)
	{
		int size = strlen(k._name) + 1;
		_name = new char[size];
		strcpy_s(_name, size, k._name);
	}
	const Person & operator=(const Person &k)
	{
		if (this != &k)
		{
			_bloodGroup = (k._bloodGroup);
			int size = strlen(k._name) + 1;
			_name = new char[size];
			strcpy_s(_name, size, k._name);
		}
		return k;
	}
	char * getName() { return _name; }
	~Person() {
		delete[] _name;
		_name = nullptr;
	}
	virtual void Info() = 0;
	BloodGroup * getBloodGroup() { return &_bloodGroup; }
};
class Donor : public Person {
	// If the phone number is not in valid format, it should be set at a specified value: 000 - 000 - 000
	string _phoneNumber; //regex: 000/000-000 ili 000-000-000
	DateD _dateOfLastDonation;
	bool _remindMe;
	bool _contactMe;
public:
	Donor(char* name, BloodGroup bloodGroup, string phoneNumber, DateD dpd, bool
		remind = true, bool contact = true)
		: Person(name, bloodGroup), _dateOfLastDonation(dpd), _remindMe(remind), _contactMe(contact) {
		if (ValidFormat(phoneNumber))
			_phoneNumber = phoneNumber;
		else
			_phoneNumber = "000-000-000";
	}
	bool getReminder() { return _remindMe; }
	bool getContactMe() { return _contactMe; }
	DateD getDate() { return _dateOfLastDonation; }

	void SetDate(DateD dat) { _dateOfLastDonation = dat; }
	bool ValidFormat(string phoneNumber)
	{
		string rule = "(\\d{3})([-/])?(\\d{3})([/-])?(\\d{3})";
		if (regex_match(phoneNumber, regex(rule)))
		{
			return true;
		}

		else
		{
			int number = 10;
			char Phone[11];
			for (size_t i = (phoneNumber.size()); i > 0; i--)
			{
				if ((int)phoneNumber[i]>46 && (int)phoneNumber[i]<58)
				{
					Phone[number--] = phoneNumber[i];
					if (number == 8 || number == 4)
					{
						Phone[number--] = '-';
					}
				}
			}
			_phoneNumber = Phone;
			cout << "Phone:" << phoneNumber << " , changed with:" << Phone << endl;
		}

	}
	void Info() {
		cout << "Donor!";
	}
};
class Request {
	string _institution;
	DateD _requestDate;
	BloodGroup _bloodGroup;
	double _amount;
public:
	Request(string institution = " ", DateD date = DateD(1, 1, 2011), BloodGroup bloodGroup = O, double amount = 0) :
		_institution(institution), _requestDate(date), _bloodGroup(bloodGroup),
		_amount(amount) { }
	friend ostream& operator<<(ostream& COUT, const Request& obj) {
		COUT << "Institution: " << obj._institution << endl;
		COUT << "Date: " << obj._requestDate.ToString() << endl;
		COUT << "Blood group: " << obj._bloodGroup.ToString() << endl;
		COUT << "Amount: " << obj._amount << endl;
		return COUT;
	}
	const BloodGroup & getBgroup() { return _bloodGroup; }
	double getAmount() { return _amount; }
	DateD getDate() { return _requestDate; }
};
class TransfusionCenter {
	// inventory status for each blood group
	Collection<BloodGroup*, double, 8> _inventory;
	//records every blood donation
	Collection<Person *, double, 100> _donations;
	vector<Request> _requests;
public:
	void AddDonation(DateD dat, Person * donor, double amount)
	{
		Donor * donorD = dynamic_cast<Donor*>(donor);
		if (donorD != NULL)
		{
			bool exist = false;
			for (size_t i = 0; i < _inventory.GetCurrently(); i++)
			{
				if (_inventory.GetElement1(i) == donorD->getBloodGroup())
				{
					_inventory.SetElement2(i, _inventory.GetElement2(i) + amount);
					exist = true; numeak;
				}
			}
			donorD->SetDate(dat);
			_donations.AddElement(donor, amount);
			if (!exist)
			{
				_inventory.AddElement(donorD->getBloodGroup(), amount);
			}
		}
	}

	bool Informed(string *array, int num, string name)
	{
		for (size_t i = 0; i <num; i++)
		{
			if (array[i] == name)
			{
				return true;
			}
		}
		return false;
	}
	void SendSMS(Request & z)
	{
		string * informed = new string[_donations.GetCurrently() + 1];
		int numOb = 0;
		for (size_t i = 0; i < _donations.GetCurrently(); i++)
		{
			if (*_donations.GetElement1(i)->getBloodGroup() == z.getBgroup())
			{
				Donor * d = dynamic_cast<Donor *>(_donations.GetElement1(i));
				if (d != nullptr)
				{
					if (d->getContactMe() == true && d->getDate().DifferenceDatesThreeMonths(z.getDate()))
					{
						string donor = _donations.GetElement1(i)->getName();
						if (Informed(informed, numOb, donor))
						{
							return;
						}
						informed[numOb++] = donor;
						cout << donor << " additional amount of your blood group needed" << endl;
					}
				}
			}
		}
	}
	void AddRequest(Request & request)
	{
		for (size_t i = 0; i < _inventory.GetCurrently(); i++)
		{
			if (*_inventory.GetElement1(i) == request.getBgroup() && _inventory.GetElement2(i) >= request.getAmount())
			{
				_inventory.SetElement2(i, _inventory.GetElement2(i) - request.getAmount());
				cout << "Successfully filled request!";
				return;
			}
		}
		thread newN(&TransfusionCenter::SendSMS, *this, (request));
		newN.join();
	}
	void GetThanksgivings(int a)
	{
		//prints thanksgivings (We are grateful to [donor's name and surname] on totally donated [X] blood doses) for TOP 2 donors 
		Collection<string, int, 100> donors;
		for (size_t i = 0; i < _donations.GetCurrently(); i++)
		{
			int TotalOneDonor = 0;
			string donor = _donations.GetElement1(i)->getName();
			for (size_t j = i; j < _donations.GetCurrently(); j++)
			{
				if (donor == _donations.GetElement1(j)->getName())
				{
					TotalOneDonor++;
				}
			}
			if (!Informed(donors.GetElements1(), donors.GetCurrently(), donor))
			{
				donors.AddElement(donor, TotalOneDonor);
			}
		}
		donors.Sort("ASC");
		for (size_t i = 0; i < a; i++)
		{
			cout << "We are grateful to " << donors.GetElement1(i) << " on totally donated " << donors.GetElement2(i) << " blood doses!" << endl;
		}
	}
};
void main() {
	DateD dat_12_01_2018(12, 1, 2018), dat_01_02_2018(1, 2, 2018);
	cout << dat_01_02_2018.ToString() << endl; //default separator is full stop ".", so it prints 1.2.2018.
	cout << dat_12_01_2018.ToString('/') << endl; //separator has been sent, so it prints 12 /1 / 2018
	Collection<int, int, 10> collection1;
	for (size_t i = 0; i < collection1.GetMax(); i++)
		if (!collection1.AddElement(i, 170000 + i))
			cout << "Elements " << i << " and " << 170000 + i << " haven't been added to the collection." << endl;
	cout << collection1.GetElement1(0) << " " << collection1.GetElement2(0) << endl;
	collection1.Sort("ASC"); //key words for sorting are ASD and DESC, and sorting is done by key element
	cout << collection1 << endl << endl << endl;
	collection1.Sort("DESC");
	cout << collection1 << endl;
	collection1.SetKeyElement(2); //sets key element on T2
	collection1.Sort("DESC"); //now sorting is done by element T2
	cout << collection1 << endl;
	BloodGroup
		O_poz(O, '+'), O_neg(O, '-'),
		A_poz(A, '+'), A_neg(A, '-'),
		B_poz(B, '+'), B_neg(B, '-'),
		AB_poz(AB, '+'), AB_neg(AB, '-');
	vector<BloodGroup> donors_O_pos{ O_poz, A_poz, B_poz, AB_poz };
	O_poz.SetDonors(donors_O_pos);
	//or shorter written
	O_poz.SetBloodRecipients(vector<BloodGroup>{O_poz, O_neg});
	A_poz.SetDonors(vector<BloodGroup>{A_poz, AB_poz});
	A_poz.SetBloodRecipients(vector<BloodGroup>{O_poz, A_poz, O_neg, A_neg});
	B_poz.SetDonors(vector<BloodGroup>{B_poz, AB_poz});
	B_poz.SetBloodRecipients(vector<BloodGroup>{O_poz, B_poz, O_neg, B_neg});
	/*
	The blood group data should be in the following format:
	-------------------------------------------
	Blood Group -> 0+
	-------------------------------------------
	Donors -> 0+, A +, B +, AB +
	Recipients -> 0+, 0-
	-------------------------------------------
	*/
	cout << O_poz.ToString() << endl;
	Person * jasmin = new Donor("Jasmin Azemovic", B_poz, "061-111-222", DateD(12, 2, 2017), true, true);
	Person * adel = new Donor("Adel Handzic", A_neg, "061-222-333", DateD(9, 1, 2017), true, true);
	Person * goran = new Donor("Goran Skondric", B_neg, "061-333-444", DateD(9, 3, 2017), true, true);
	TransfusionCenter tcMostar;
	/*
	each donation needs to increase stockpiles for that blood group
	If a blood group already exists on the stock then only its amount is increased
	Also, donor sets a new value for the last donation date
	*/
	//donation date, donor, amount
	tcMostar.AddDonation(DateD(20, 5, 2017), jasmin, 2.5);
	tcMostar.AddDonation(DateD(20, 5, 2017), adel, 3);
	tcMostar.AddDonation(DateD(6, 5, 2017), goran, 1.2);
	tcMostar.AddDonation(DateD(10, 9, 2017), jasmin, 2);
	tcMostar.AddDonation(DateD(18, 10, 2017), adel, 1.8);
	tcMostar.AddDonation(DateD(15, 9, 2017), goran, 3.8);
	Request request_0_poz("Trasfusion center KCUS", DateD(18, 2, 2018), O_poz, 15),
		request_0_neg("Trasfusion center Bakir Nakas", DateD(20, 2, 2018), O_neg, 8),
		neki("something", DateD(1, 1, 2018), B_poz, 9);
	/*if the transfusion center holds the required amount in the state, it will immediately
submit them to the request,
and in case they do not possess then they will (using multithreading) contact
(send SMS with the appropriate content)
all donors that meet the following conditions:
- can donate blood to the required blood group
- they indicated they wanted to be contacted
- They did not give blood in the last 3 months
	*/
	tcMostar.AddRequest(request_0_poz);
	tcMostar.AddRequest(request_0_neg);
	tcMostar.AddRequest(neki);
	//prints thanksgivings (We are grateful to [donor's name and surname] on totally donated [X] blood doses) for TOP 2 donors 
	tcMostar.GetThanksgivings(2);
	delete adel;
	delete jasmin;
	delete goran;

	system("pause>FIT");
}