//project code skeleton

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std; 

class website
{
private: 
	string link; 
	
	vector<string> keywords;
	
	float curr_page_rank; 
	float page_rank_normalized; 
	float prev_page_rank;
	float rank;
	float CTR; 
	float score; 
	
	int outgoing_links_number;
	int clicks; 
	int impressions; 
	int ID; 


public:
	vector<int> ids_pointing_to_me; //would be used as a list of nodes that reference this website so that we can calculate the pr easily

	website(string link_str, int id, int outgoing_links_no){
		ID = id; 
		link = link_str;
		outgoing_links_number = outgoing_links_no;
	}
	int get_impressions(){
		return impressions;
	} 
	string get_link(){
		return link;
	}
	void set_score(float sc) {
		score = sc;
	}
	void set_page_rank(float pr){
		curr_page_rank = pr;
	}
	float get_page_rank(){
		return curr_page_rank;
	}
	float get_prev_page_rank(){
		return prev_page_rank;
	}
	void prv_equ_cur_pr(){
		prev_page_rank = curr_page_rank;
		curr_page_rank = 0;
	}

	int get_ID(){
		return ID;
	}
	void set_impressions(int x){
		impressions = x;
	}
	void inc_impressions(){
		impressions++;
	}
	void set_clicks(int x){
		clicks = x;
	}
	void inc_clicks(){
		clicks++;
	}
	float get_score(){
		return score;
	}

	void calculate_CTR(){
		CTR = clicks / impressions;
	}
	double get_CTR(){
		return CTR;
	}
	int get_clicks(){
		return clicks;
	}
	void set_outgoing_links(int x);
	void inc_outgoing_links(){
		outgoing_links_number++;
	};
	int get_outgoing_links(){
		return outgoing_links_number;
	}
	void add_id_pointing_to_me(int x){
		ids_pointing_to_me.push_back(x);
	}
	void push_back_keyword(string word){
		keywords.push_back(word);
	}

	void set_pr_normalized(float max_Pr){
		page_rank_normalized = prev_page_rank / max_Pr;
	}
	double get_pr_normalized(){
		return page_rank_normalized;
	}

	~website(){};
	
};




class SearchEngine
{
private: 
	string WebGraphPath; 
	string KeywordsPath; 
	string ImpressPath;  
	string ClicksPath;

	fstream WebGraphFile;
	fstream KeywordsFile; 
	fstream ImpressFile; 
	fstream ClicksFile; 

	map<string, int> link_to_id;
	map<int, website*> id_to_website;
	map<string, vector<int>*> keywords_to_websites;

	int number_of_websites;

	int ID; 
public:

	SearchEngine(string web_graph_path, string keywords_path, string impressions_path, string clicks_path);
	void initialize();
	void load_keywords();
	void load_impressions();
	void load_clicks();
	void calculate_page_rank();
	void calculate_page_score();
	void calculate_pr_normalization();
	void sort_keywords_to_websites();
	void intialize_page_rank();
	vector<int> or_handler(vector<string> keywords);
	vector<int> and_handler(vector<string> keywords);
	void search_handler();
	double max_Page_Rank;
	~SearchEngine();
};
SearchEngine::SearchEngine(string web_graph_path, string keywords_path, string impressions_path, string clicks_path){
	
	number_of_websites = 0;
	WebGraphPath = web_graph_path;
	max_Page_Rank = 0;
	KeywordsPath = keywords_path; 
	ImpressPath  = impressions_path;
	ClicksPath = clicks_path;
	ID = 0;
	/*---Opens necessary files that we are going to need in the search engine*/
	WebGraphFile.open(WebGraphPath, ios::out | ios::in );
	KeywordsFile.open(KeywordsPath, ios::out | ios::in );
	ImpressFile.open(ImpressPath,   ios::out | ios::in );
	ClicksFile.open(ClicksPath, ios::out | ios::in );
	/*---Checking if files were opened successfully*/
	bool FileFailed = false; 
	if (!WebGraphFile.is_open())
		{ cerr << "\nCould not open the Web Graph file\n"; FileFailed = true; }
	if (!KeywordsFile.is_open())
		{ cerr << "\nCould not open the keywords file\n"; FileFailed = true; }
	if (!ImpressFile.is_open())
		{ cerr << "\nCould not open the impressions file\n"; FileFailed = true; }
	if (!ClicksFile.is_open())
		{ cerr << "\nCould not open the Clicks file\n"; FileFailed = true; }

	if (FileFailed)
	{
		cerr << "File handling error\n Terminating Now\n";
		exit(1);
	}

}
void SearchEngine::initialize(){
	
	int lines_count = 0;
	std::string line;

	while (std::getline(WebGraphFile, line))
	{
    	std::istringstream iss(line);
    	string src;		//the website that is mentioning another website 
		string dest;	//the website that is being referenced 
    	
    	getline(iss, src, ',');
    	getline(iss, dest);
    	//Now I want to assign each link an ID, so we check if it exists or not, and if not, we assign it a new ID
    	if ( link_to_id.find(src) == link_to_id.end() ) {
		  	//if the link doesn't exist
		  	int id = ID;
		  	link_to_id.insert(make_pair(src, id)); 
		  	website* Wbsite = new website(src, id, 1);
		  	id_to_website.insert(make_pair(id, Wbsite));
		  	ID++;
		  	number_of_websites++;
		} else {
			//this means that that website was encounterd before (either as a source or as a destintation)
			int website_id = link_to_id[src]; //returns the ID of the website 
			website* wbs = id_to_website[website_id]; //returns a pointer to the website 
			//now we can increase the outgoing links of that website by 1 as it is referencing another website 
			wbs->inc_outgoing_links();   	
		}

		if ( link_to_id.find(dest) == link_to_id.end() ) {
		  	//if the link doesn't exist
		  	int id = ID;
		  	link_to_id.insert(make_pair(dest, id)); 
		  	website* Wbsite = new website(dest, id, 0);
		  	Wbsite->add_id_pointing_to_me(link_to_id[src]);
		  	id_to_website.insert(make_pair(id, Wbsite));
		  	ID++;
		  	number_of_websites++;
		} else {
			//this means that that website was encounterd before (either as a source or as a destintation)
			int website_id = link_to_id[dest]; //returns the ID of the website 
			website* wbs = id_to_website[website_id]; //returns a pointer to the website 
			//now we can append the source website id to a list that we would iterate over at the end to find the pageRank 
			wbs->add_id_pointing_to_me(link_to_id[src]);   	
		}

		lines_count++;

	}
//cout  <<"\n We found " << number_of_websites << " websites\n"; 

/*for(auto it = link_to_id.cbegin(); it != link_to_id.cend(); ++it)
{
    std::cout << "link: "<< it->first << " to id: " << it->second << "\n";
}*/
	//now we have set up each website as a node in the map, and if it was a destination, we added a vector of the ids of other websites that 
	//are pointing to it 
	//Now, we still need to load the impressions as well as the keywords 
	load_keywords();
	load_impressions();
	load_clicks();
	intialize_page_rank();
	calculate_page_rank();
	calculate_pr_normalization();
	calculate_page_score();
	sort_keywords_to_websites();
	search_handler();

	return;
}

void SearchEngine::load_keywords(){
	string line; 
	while(getline(KeywordsFile,line)){
		istringstream ss(line); 
		string extracted_link;
		getline(ss, extracted_link, ',');
		//link is a string, get its id and access its website
		int link_id = link_to_id[extracted_link];
		website* ws = id_to_website[link_id];
		string word;

		while(getline(ss, word, ',')){
			
			if (keywords_to_websites.find(word) != keywords_to_websites.end())
			{
				//keyword was seen before
				//thus, there is an entry for it, we just need to get the pointer to the vector of websites ID
				// and push the ID of the website we have now
				keywords_to_websites[word]->push_back(link_id);
			} 
			else 
			{
				//keyword is new, thus, we need to make a vector of int pointer and pass it to the vector and push the id of the website to it
				vector<int>* ids = new vector<int>;
				ids->push_back(link_id);
				keywords_to_websites[word] = ids;
			}

			ws->push_back_keyword(word);
		}
	}
}

void SearchEngine::load_impressions(){
	string line;
	while(getline(ImpressFile, line)){
		string extracted_link; 
		istringstream ss(line);
		getline(ss, extracted_link, ',');
		string impressions_str; 
		getline(ss, impressions_str);
		int link_id = link_to_id[extracted_link];	
		website* ws = id_to_website[link_id];
		ws->set_impressions(std::stoi(impressions_str));

	}
}
void SearchEngine::load_clicks(){
	string line;
	
	while(getline(ClicksFile, line)){
		string extracted_link; 
		istringstream ss(line);
		getline(ss, extracted_link, ',');
		string clicks_str; 
		getline(ss, clicks_str);

		int link_id = link_to_id[extracted_link];	
		website* ws = id_to_website[link_id];
		ws->set_clicks(std::stoi(clicks_str));

	}
}
void SearchEngine::intialize_page_rank(){
	//first, we need to iterate over the id_to_website map and intialize the PR of all websites to the same value (1/number_of_websites)

	map<int, website*>::iterator itr;
	float rank = 1.0 / (float)number_of_websites;

	for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) { 
        itr->second->set_page_rank(rank);
    }

}

void SearchEngine::calculate_page_rank(){
	//first, we need to iterate over the id_to_website map and intialize the PR of all websites to the same value (1/number_of_websites)

	map<int, website*>::iterator itr;

	for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) 
		{	itr->second->prv_equ_cur_pr(); }

	for (int i = 0; i < 5; i++)
	{
		for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) 
		{ 
			//double rank_temp = itr->second->get_prev_page_rank(); 
        	float rank_temp = 0;
        	//cout << "\nFor website " << itr->second->get_link() << " it is pointed to by: ";
        	for (int u : itr->second->ids_pointing_to_me)
        	{
        		//u now has the id of the nodes that are pointing to my node 
        		rank_temp += ((float)(id_to_website[u]->get_prev_page_rank())/(float)(id_to_website[u]->get_outgoing_links()));
        		//cout << id_to_website[u]->get_link() << " -- ";
        	}
        	//cout << endl;
        	//cout << rank_temp << "  rank of website " << itr->second->get_link()<< "\n";
        	itr->second->set_page_rank(rank_temp);

        	if (rank_temp > max_Page_Rank) max_Page_Rank = rank_temp;
    	}

    	for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) 
		{
        	itr->second->prv_equ_cur_pr();
    	}
    	//cout << endl;
	}
	for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) 
		{
        	//cout << "rank of " << itr->second->get_link() << " is " << itr->second->get_prev_page_rank() << endl;
    	}
}
void SearchEngine::calculate_pr_normalization(){
	map<int, website*>::iterator itr;
	for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) 
		{
        	itr->second->set_pr_normalized(max_Page_Rank);
    	}
}

void SearchEngine::calculate_page_score(){
	float score = 0;
	map<int, website*>::iterator itr;
	
	for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) 
		{
        	score = 0;
        	float pr_normalized = itr->second->get_pr_normalized();
        	float first_term = 0.4 * pr_normalized;
        	float ctr = itr->second->get_CTR();

        	float impressions = itr->second->get_impressions();
        	float impressions_fraction = (0.1 * impressions) / (1.0 + 0.1 * impressions); 
        	float sec_term = 	0.6 * (  (1-impressions_fraction) * pr_normalized + impressions_fraction * ctr);

        	float score = first_term + sec_term;
        	itr->second->set_score(score);
    	}
}
void SearchEngine::sort_keywords_to_websites(){
	//for my method to work, we need to sort the vector of IDs that is mapped to keywords of all possible keywords
	   
	   map<string, vector<int>*>::iterator it = keywords_to_websites.begin();
	   while(it != keywords_to_websites.end())
	   {
	   	vector<int>* ids_pointer = it->second;
	   //	cout << it->first << endl;
	   	std::sort (ids_pointer->begin(), ids_pointer->end());  
	   	it++;
	   }
	   return;
}
bool sortByPageRank(website* lhs, website* rhs){
	return lhs->get_prev_page_rank() < rhs->get_prev_page_rank();
}

void SearchEngine::search_handler(){
/*-----	Handling user input ------*/

	int input; 
	cout << "Welcome!\nWhat would you like to do?\n";
	cout << "1- New search \n2- Exit\n";
	cout << "Type in your choice: ";
	cin >> input;
	
	if (input == 1)
	{
		search_again:
		string search;
		cout << "Enter your query: ";
		cin.ignore();
		vector<string> search_keys;
		getline(cin, search);
		std::stringstream ss(search);
	    std::string s;
		string OR  = "OR";
		string AND = "AND";
		bool isOR = true; 
		bool isAND = false;

	    while (ss >> std::quoted(s))
	    {
			int res1 = s.compare(OR);
			int res2 = s.compare(AND);
			
	       if (res1 == 0 || res2 == 0 )
	       {
			   if (res1 == 0) { isOR = true; }
			   if (res2 == 0) { isAND = true; isOR = false;}
	       }
		   else { search_keys.push_back(s); }
	    }
		if (isAND) {
			isOR = false;  
			//cout << "\nWE are searching with AND\n";
		}

		//if (isOR) cout << "\nWE are searching with OR\n";
		
		/*for (int i = 0; i < search_keys.size(); i++)
			cout << "keyword #" <<i+1 <<" " << search_keys[i] << endl;*/
		
		vector<int> ids_of_search_websites;
		
		if (isOR)
		{
			 ids_of_search_websites = or_handler(search_keys);
		}
		else{
			 ids_of_search_websites = and_handler(search_keys);
		}
		
		if (ids_of_search_websites.size() == 0)
		{
			cout << "\nNo results were found\n";
		}

		//now sort ids_of_search_websites by their page rank 
		std::vector<website*> search_result;

		for (auto i = ids_of_search_websites.begin(); i != ids_of_search_websites.end(); ++i)
    			{
    				website* t = id_to_website[*i];
    				std::cout << t->get_link() << '\n';
    				search_result.push_back(id_to_website[*i]);
    			}

    	std::sort(search_result.begin(), search_result.end(), [](website* lhs, website* rhs) {
      return lhs->get_score() > rhs->get_score();
   });

		display_search_results:
		cout << "\nSearch results: \n";
		int i = 1;
		for (website* n : search_result)
		{
        	cout << i++ << ". "<<n->get_link() << endl;// << " and its score is : "<< n->get_score() << " \n";
    		n->inc_impressions(); 
    	}

    	cout << "\nWould you like to do: ";
    	cout << "\n1. Choose a webpage to open";
    	cout << "\n2. New search:";
		cout << "\n3. Exit \n";
		cout << "\nType in your choice: ";
		int choice; cin >> choice;

		if (choice == 3)
		{
			return;
		}
		if (choice == 2)
		{
			//search_handler();
			goto search_again;
		}
		if (choice == 1)
		    	{
		    		int number;
		    		cout << "Enter the number of the website that you want to open: ";
		    		cin >> number;
		    		number--;
		    		//cout <<"Incrementing clicks of " <<search_result[number]->get_link()<<endl;
		    		cout << "You are now viewing " << search_result[number]->get_link()<<endl;
		    		search_result[number]->inc_clicks();
		    		map<int, website*>::iterator itr;
		
		    		//cout << "Now it was clicked: " << search_result[number]->get_clicks() << " times\n";
		    		//fstream clicks_updated("click2.csv");
		    		//fstream impressions_updated("impressions2.csv"); 

		    			ClicksFile.close();
		    			ImpressFile.close();
    					ofstream ClicksFile("./clicks.csv", ios::trunc);
    					ofstream ImpressFile("./impressions.csv", ios::trunc);
    						
		    			for (itr = id_to_website.begin(); itr != id_to_website.end(); ++itr) { 
        					//cout << itr->second->get_link() << " " << itr->second->get_clicks() << endl;
        				//	clicks_updated << itr->second->get_link() << "," << itr->second->get_clicks()<<"\n";
        					ClicksFile << itr->second->get_link() << "," << itr->second->get_clicks()<<"\n";
        					ImpressFile<< itr->second->get_link() << "," << itr->second->get_impressions()<<endl;
        				//	impressions_updated << itr->second->get_link() << "," << itr->second->get_impressions()<<endl;
    					}

    					/*ClicksFile.close();
    					ofstream ClicksFile("./clicks.csv");
    					ClicksFile << clicks_updated.rdbuf();*/
    					cout << "\nWould you like to: ";
				    	cout << "\n1. Go Back to search results: ";
				    	cout << "\n2. New search:";
						cout << "\n3. Exit \n";
						cout << "\nType in your choice: ";
						int choice2; cin >> choice2;

						if (choice2 == 3)
							return;
						if (choice2 == 2)
							goto search_again;
							//search_handler();
						if(choice2 == 1)
							goto display_search_results;
					

		    	}    	

	}
	else{
		cout << "\nGoodbye!\n";
		return;
	}
}
vector<int>  SearchEngine::or_handler(vector<string> keywords){
	//we need to loop over the keywords, extract the vector of ints of websites IDs for each of them,
	// and then merge them using set_union in a new vector and return this vector of IDS, 
	//which can later be converted to strings using the id to website map
vector<int> search_return_id;

	for (int i = 0; i < keywords.size(); ++i)
	{
		//merge each two together after getting their vectors 
		//check if keyword exists first 
		if ( keywords_to_websites.find(keywords[i]) != keywords_to_websites.end() ) {
		  
		std::vector<int>* websites1 = keywords_to_websites[keywords[i]];
		std::vector<int> temp;
		set_union(search_return_id.begin(), search_return_id.end(), websites1->begin(), websites1->end(), back_inserter(temp));
		search_return_id.clear();
		sort(temp.begin(), temp.end());
		search_return_id = temp;
		}
		
		
	}

return search_return_id;

}
vector<int> SearchEngine::and_handler(vector<string> keywords){
vector <int> search_return_id;
	if ( keywords_to_websites.find(keywords[0]) != keywords_to_websites.end()) 
	{
	vector <int>* t = keywords_to_websites[keywords[0]];
	
		for (int i=0; i<t->size(); i++) 
	        {
	        	//cout << id_to_website[t->at(i)]->get_link() <<" ya mina " << endl;
	        	search_return_id.push_back(t->at(i)); 
	        }
    

	for (int i = 1; i < keywords.size(); ++i)
	{
		if ( keywords_to_websites.find(keywords[i]) != keywords_to_websites.end()) 
	{
		//merge each two together after getting their vectors 
		std::vector<int>* websites1 = keywords_to_websites[keywords[i]];
		std::vector<int> temp;
		set_intersection(search_return_id.begin(), search_return_id.end(), websites1->begin(), websites1->end(), back_inserter(temp));
		//search_return_id->clear();
		sort(temp.begin(), temp.end());
		search_return_id = temp;
	}}
	}

return search_return_id;

}

SearchEngine::~SearchEngine(){
	WebGraphFile.close();
	KeywordsFile.close();
	ImpressFile.close();
	ClicksFile.close();
}




int main(int argc, char const *argv[])
{

/*------      Files paths                  ------------*/
	string web_graph_path ("./web_graph.csv"); 			//the web graph file path 
	string keywords_path ("./keyword.csv"); 			//the keyowrds of each website file path 
	string impressions_path ("./impressions.csv"); 		//the number of impressions file path 
	string clicks_path ("./clicks.csv");

/*-----       Starting the search engine   ------------*/	
	SearchEngine search_engine(web_graph_path, keywords_path, impressions_path, clicks_path);	

	search_engine.initialize();


	
	return 0;
}
