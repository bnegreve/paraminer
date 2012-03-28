#if !defined __GLCM_CPP__
#define      __GLCM_CPP__

#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <sys/time.h>
#include <sstream>
#include <iomanip>

#include "TransactionSequence.h"
#include "ItemSet.h"
#include "ItemSet.h"
#include "GLCM.hpp"
#include "DBMain.h"
#include "BinaryMatrix.h"
#include "TimeTracker.h"

using namespace std;
using namespace nsPattern;

typedef struct
{
	int ind;
	int val;
} SortedObject;

int SortFunction (const void * a, const void * b)
{
	SortedObject * sa = (SortedObject *) a;
	SortedObject * sb = (SortedObject *) b;
	return (sa->val - sb->val);
}

bool funcSortTI (TransactionSequence TS1, TransactionSequence TS2) {
	  	  return TS1.getTransactionAt(1) < TS2.getTransactionAt(1);
};

struct SortLOPair
{
    bool operator()(pair<int,int> pair1, pair<int,int> pair2)
	{
		return pair1.second < pair2.second;
	};
};

GLCM::~GLCM(){} ;

/////////////////////////////////////////////////////////////////////////////////
//////////    Compute before Execute Algorithm   ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

vector <TransactionSequence> GLCM::buildG_1item(DBMain * DB)
{
	//clock_t start_time, end_time,temps;
	time_t t1, t2;

	t1 = time((time_t *)NULL);

	vector <TransactionSequence> vTS;
	TransactionSequence tempTS = TransactionSequence();
	int * All_Transaction; //= new int [DB->GetCustomer()];

	int nbAtt = DB->GetAttribute();
	int nbCus = DB->GetCustomer();
	SortedObject * ValCopy = (SortedObject *) malloc (nbCus * sizeof (SortedObject));

	for (int i = 0 ; i < 2 * nbAtt; i++)
	{
		All_Transaction = DB->GetDimension(i/2);
		vector <SortedObject *> tvTS;
		for (int j = 0 ; j < nbCus; j++)
		{
			/*TransactionSequence * ts = new TransactionSequence();
			ts->clear();
			ts->addTransactionBack(j);
			ts->addTransactionBack(All_Transaction[j]);*/
			/*TransactionSequence ts = TransactionSequence();
			ts.clear();
			ts.addTransactionBack(j);
			ts.addTransactionBack(All_Transaction[j]);
			tvTS.push_back(ts);*/
			//delete ts;
			ValCopy[j].ind = j;
			ValCopy[j].val = All_Transaction[j];
			//tvTS.push_back(ValCopy);
		}

		if (i % 2 == 0)//item increase
		{
			//sort(tvTS.begin(),tvTS.end(), funcSortTI);
			qsort (ValCopy, nbCus, sizeof (SortedObject), SortFunction);
			tempTS.clear();
			//list<TransactionSequence>::iterator it;
			for (int k = 0; k < nbCus; k++) //(signed)tvTS.size()
			{
				//TransactionSequence te = tvTS[i];
				//cout << ValCopy[k].ind << endl;
				tempTS.addTransactionBack(ValCopy[k].ind); //te.getTransactionAt(0)
			}
			//tempTS.sortSEQ(0);
		}
		else
		{
			tempTS.reverseTransactionSeqOrder();
		}

		
		vTS.push_back(tempTS);
	}
	t2 = time((time_t *)NULL);
	//cout<<"debut Build G 1 item  ===>  "<<ctime(&t1)<<endl;
	//cout<<"fin Build G 1 item  ===>  "<<ctime(&t2)<<endl;
	//delete All_Transaction;
	//delete tempTS;
	free(ValCopy);
	return vTS;
};

/*BinaryMatrix GLCM::constructBinaryMatrix(int item, vector<TransactionSequence> & G1Item)
{
	TransactionSequence ts = G1Item[item];
	BinaryMatrix bm = BinaryMatrix(ts.size());
	//bm.Initialize(ts.size());

	for (int i = 0; i < ts.size(); i++)
	{
		for (int j = 0; j < ts.size(); j++)
		{
			int col = ts.getTransactionAt(i);
			int row = ts.getTransactionAt(j);
			if (j > i)
				bm.setValue(row,col,1);
			else bm.setValue(row,col,0);
		}
	}
	bm.PrintInfo();
	cout << endl;
	return bm;
}*/

vector<BinaryMatrix *> GLCM::constructBinaryMatrixAll(vector<TransactionSequence> G1Item)
{
	//clock_t start_time, end_time,temps;
	time_t t1, t2;

	t1 = time((time_t *)NULL);
	vector<BinaryMatrix *> vBM;// = new vector<BinaryMatrix *>();
	vBM.clear();
	int G1ItemSize = G1Item.size();
	for (int i = 0; i < G1ItemSize; i++)
	{
		BinaryMatrix BM = BinaryMatrix(G1Item[i].size());
		BM.constructBinaryMatrix(i,G1Item);
		//BM->PrintInfo();
		//BinaryMatrix * BM1 = new BinaryMatrix(BM);
		vBM.push_back(new BinaryMatrix(BM));
		//delete BM;
		//delete BM1;
	}
	t2 = time((time_t *)NULL);
	//cout<<"debut build Matrix ===>  "<<ctime(&t1)<<endl;
	//cout<<"fin build Matrix  ===>  "<<ctime(&t2)<<endl;
	return vBM;
}

//Old Frequent
//Get max chain from 1 transaction.
void GLCM::findMaxChain(int transaction, BinaryMatrix * BM, TransactionSequence & ts)
{
	ts.addTransactionBack(transaction);
	//cout << "Check zero" << ts << BM->checkZero(transaction) << endl;
	if (BM->checkZero(transaction)) return;
	int maxChild = BM->getMaxChild(transaction);
	if (maxChild != -1) findMaxChain(maxChild, BM, ts);
}

//Find max Transaction Sequence of an Item Set
void GLCM::findMaxTranSeq(BinaryMatrix * BM, TransactionSequence & ts)
{
	ts.clear();
	int BMSize = BM->getSize();
	if (BMSize == 0) return;
	int maxTrans = 0;
	//maxTrans = BM->getMaxTrans();
	for (int i = 0; i < BMSize; i++)
	{
		if (BM->checkRootTree(i)) findMaxChain(i,BM,ts);
	}
	cout << ts.size()<< "ts = " << ts << endl;
}

//Check one Sequence Frequent or not
bool GLCM::CheckFrequent(BinaryMatrix * BM, int threshold, string freType, TransactionSequence & ts)
{
	if (BM->getSize() == 0) return false;
	if (freType == "nor")
	{
		//BM->PrintInfo();
		//TransactionSequence ts;
		findMaxTranSeq(BM, ts);
		//cout << ts.size() << endl;
		//cout << threshold << ts << endl;
		return  ts.size() >= threshold;
	}
	else
	{
		/*BM->PrintInfo();
		cout << BM->getActive()<< endl;*/
		return (BM->getActive()) >= threshold;
	}
}

///////////////////////////////////////////////////////////////////
/////////////////  CAL F   ////////////////////////////////////////
///////////////////////////////////////////////////////////////////

ItemSet GLCM::calF(BinaryMatrix * BM, vector<TransactionSequence> G1Item, const vector<BinaryMatrix *> & vBM, string freType, int & resFre)
{

	ItemSet is = ItemSet();
	is.clear();
	//TransactionSequence ts;
	int BMSize = BM->getSize();
	BinaryMatrix calFBM(BMSize);
	calFBM = (*BM);
	vector<int> freMap;
	freMap.clear();
	for (int i = 0; i < BMSize; i++)
	{
		freMap.push_back(-1);
	}
	loop_Chk_Freq(calFBM,freMap);
	resFre = frequentCount(freMap);
	if (resFre < _threshold) return is;

	bool change = false;
	reduceBM(calFBM, resFre, change);
	//bool bcf = CheckFrequent(BM, _threshold, freType, ts);
	//if (!bcf) return is;


	int G1ItemSize = G1Item.size();
	for (int i = 0; i < G1ItemSize; i++)
	{
		//if (vBM[i]->isInclude(BM)) is.addItem(i);
		BinaryMatrix tBM = (*vBM[i]);
		tBM &= (*BM);
		if (tBM == (*BM)) is.addItem(i);
	}
	return is;
}

///////////////////////////////////////////////////////////////////
///////////////// CAL G  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void GLCM::calG(ItemSet is, const vector<BinaryMatrix *> & vBM, BinaryMatrix& res)
{
	int isSize = is.size();
	if (isSize == 0)
        return;

    res = *vBM[is.getItemAt(0)];

	for (int i = 1; i < isSize; i++)
        res &= *vBM[is.getItemAt(i)];
}

BinaryMatrix * GLCM::andMatrix(BinaryMatrix * bm1, BinaryMatrix * bm2)
{
	BinaryMatrix * tBM = new BinaryMatrix(bm1->getSize());
	for (int i = 0; i < bm1->getSize(); i++)
	{
		for (int j = 0; j < bm1->getSize(); j++)
		{
			if ((bm1->getValue(j,i) == 1) && (bm2->getValue(j,i) == 1)) tBM->setValue(j,i,1);
			else tBM->setValue(j,i,0);
		}
	}
	return tBM;
}
///////////////////////////////////////////////////////////////////
//////////////  GLCM   ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void GLCM::setThreshold(int th)
{
	_threshold = th;
}

void GLCM::findP_i_minus_1(ItemSet P, int i, ItemSet& res)
{
//	ItemSet * tempIS = new ItemSet();
//	tempIS->clear();
    res.clear();
	if (i == -1) //return tempIS;
        return;
	if (P.size() != 0)
	{
		for (int k = 0 ; k < P.size() ; k++)
		{
			if (P.getItemAt(k) == i) return;// tempIS;
			else res.addItem(k);
		}
	}
//	return tempIS;
}

int GLCM::findCore_i(ItemSet P, vector<TransactionSequence> G1Item, vector<BinaryMatrix *> & vBM)
{

//	BinaryMatrix * BM = new BinaryMatrix();
//	BM = calG(P,vBM);

    //	BinaryMatrix * BM = calG(P,vBM);
    BinaryMatrix    BM(vBM[0]->getSize());
    calG(P, vBM, BM);

	int core_i = -1;
//	BinaryMatrix * tempBM; // = new BinaryMatrix();
	if (P.size() != 0)
	{

		ItemSet tempIS = ItemSet();
		tempIS.clear();

        BinaryMatrix    tempBM(vBM[0]->getSize());
		for (int i = 0 ; i < P.size() ; i++)
		{
			int k = P.getItemAt(i);

			tempIS.addItem(k);

            calG(tempIS,vBM,tempBM);
			if (BM == tempBM)
                core_i = k;
		}
	}
	//cout << "find core i" << core_i <<  endl;
	//delete tempBM;
//	delete BM;
	return core_i;
}


void GLCM::dumpItemset(std::ostream &os, ItemSet itemset, int freq){
	os << "[ ";
	for (int i = 0 ; i < itemset.size() ; i++)
	{
		if ((itemset.getItemAt(i) % 2) == 0) os << itemset.getItemAt(i)/2 +1 << "+ ";
		else os << itemset.getItemAt(i)/2 +1 << "- ";
	}
	os << "] ";
  os<<"("<<freq<<")\n"; 
}

//////////////////////////////////////////////////////////////////////////
//Testttttttt
/////////////////////////////////////////////////////////////////////////
void GLCM::reduceBM(BinaryMatrix & BM, int threshold, bool & change)
{
	int maxTrans = 0;
	int maxCount = 0;
	//if (change) return;
	maxTrans = BM.getMaxTrans(maxCount);
	if (maxCount < (threshold-1)) 
	{
		BM.setBMtoZero();	
		return;
	}
	BM.reduce_nonSupport_TS(threshold,change);
	while (change) 
	{		
		BM.reduce_nonSupport_TS(threshold,change);
		//cout << "....." << change << endl;
	}
	//reduceBM(BM, threshold,change);
}

void GLCM::recursion_Chk_Freq(int trans, BinaryMatrix BM, vector<int> & freMap)
{
	int BMSize = BM.getSize();
	if (BM.checkZero(trans)) freMap[trans] = 1;
		for(int j = 0; j < BMSize; j++)
		{
			if (BM.getValue(j,trans))
			{
				//if (BM.checkZero(j)) freMap[trans] = 1;
				if (freMap[j] == -1) recursion_Chk_Freq(j,BM,freMap);
				freMap[trans] = (freMap[trans]>freMap[j] + 1)?freMap[trans]:(freMap[j] + 1);
			}
		}
}

void GLCM::loop_Chk_Freq(BinaryMatrix BM, vector<int> & freMap)
{
	int freMapSize = freMap.size();
	for(int i = 0; i < freMapSize; i++)
	{
		recursion_Chk_Freq(i,BM,freMap);
	}
}

int GLCM::frequentCount(vector<int> & freMap)
{
	int maxFreq = 0;
	int freMapSize = freMap.size();
	for(int i = 0; i < freMapSize; i++)
	{
		if (freMap[i] > maxFreq) maxFreq = freMap[i]; 
	}
	return maxFreq;
}


void GLCM::GLCMloop(ItemSet P, BinaryMatrix& res, vector<TransactionSequence> G1Item, const vector<BinaryMatrix *> & vBM, string freType, string so , std::ostream &output, int & resFre)
{
    //BinaryMatrix BM(vBM[0]->getSize());
    //calG(P, vBM, BM);
//	BinaryMatrix * BM = calG(P,vBM);
    //int fre = findMaxTranSeq(&BM);
	//TransactionSequence ts;
	//bool check = CheckFrequent(&res, _threshold, freType, ts);
	//res.PrintInfo();
	/*int BMSize = res.getSize();
	vector<int> freMap;
	freMap.clear();
	for (int i = 0; i < BMSize; i++)
	{
		freMap.push_back(-1);
	}
	loop_Chk_Freq(res,freMap);
	int kkkk = frequentCount(freMap);*/
	//cout << "kkkk = " << kkkk << endl;

	//res.PrintInfo();
	if ((P.size() != 0) && (resFre < _threshold))//(!check))
	{
		return;
	}
	//res.PrintInfo();
	//res.reduce_nonSupport_TS(ts.size());
	//bool change = false;
	//reduceBM(res, kkkk,change);
	if (P.size() != 0)
	{
		//if (fre <_threshold) return;
		//if (((P.getItemAt(0))%2) != 0)return;//Reduce opposite ItemSet
		//if (P.size() == 1) cout << P << "-->" << ts.size() << endl;
		if (resFre != G1Item[0].size()){
			//if (so == "y") ClosedItemSetList.push_back(P); //reduce 100% frequent ItemSets if (fre != G1Item[0].size())
		
		//MaxlengthTransactionSequenceList.push_back(ts);
		//BinaryMatrixList.push_back(BM);
		//cout << P << "-->" << ts.size() << endl;
		dumpItemset(output,P,resFre);
		nbIS++;
		//if ((nbIS % 1000) == 0) cout << nbIS << endl;
		}
	}
	int k = 0;
	int core_i = -1;
	if (P.size() != 0) core_i = P.getCore_i(); //findCore_i(P,G1Item,vBM);

	if ((core_i % 2 != 0) || core_i == -1) k = core_i;
	else k = core_i + 1; //reduce unnecessary items
	//BinaryMatrix * calGBM;// = new BinaryMatrix();

    BinaryMatrix calGBM(vBM[0]->getSize());
    //ItemSet P_minus_1, Q_minus_1;
	int G1ItemSize = G1Item.size();
	for (int i = k + 1; i < G1ItemSize; i++)
	{
		//P.addItem(i);
		ItemSet P1 = P.addItemwCore_i(i);
		if (P != P1)
		{
			//calG(P1,vBM,calGBM);

			calGBM = (*vBM[i]);
			if (res.getSize() != 0)
				calGBM &= res;
			//		calGBM = calG(P,vBM);
			//cout << "Transaction for Q" << endl;
			//Print_An_Itemset(P);
			//calGBM->PrintInfo();
			int resourceFre = 0;
			ItemSet Q = calF(&calGBM,G1Item,vBM,freType,resourceFre);
			if (Q.size() != 0)
			{
				Q.setCore_i(P1.getCore_i());

				//findP_i_minus_1(P,i,P_minus_1);
				//findP_i_minus_1(Q,i,Q_minus_1);

				//		ItemSet * P_minus_1 = findP_i_minus_1(P,i);
				//		ItemSet * Q_minus_1 = findP_i_minus_1(Q,i);
				if (Q.prefixEqual(P1))//((P_minus_1 == Q_minus_1) || ((P_minus_1.size() == 0) && (Q_minus_1.size() == 0)))
				{
					//delete P_minus_1;
					//delete Q_minus_1;
					GLCMloop(Q, calGBM, G1Item, vBM, freType, so, output, resourceFre);
				}
				//		delete P_minus_1;
				//		delete Q_minus_1;
				//P.deleteItemBack();
			}
		}
	}
}

void GLCM::GLCMAlgo( DBMain * DB, float threshold, string freType,string so, std::ostream &output)
{
    TimeTracker tt1;
    tt1.Start();
	nbIS = 0;
	ItemSet P = ItemSet();
	P.clear();
	ClosedItemSetList.clear();
	MaxlengthTransactionSequenceList.clear();
	BinaryMatrixList.clear();
	vector<TransactionSequence> G1Item = buildG_1item(DB);
	cout << "There are " << G1Item[0].size() << " customers, and " <<  G1Item.size()/2 << " attributes." << endl << endl;
	if (freType == "nor")
	{
		//cout << (int) ceil(threshold * G1Item[0].size()) << endl;
		_threshold = (int) ceil(threshold * G1Item[0].size());
	}
	else if (freType == "gra")
	{
		//cout << (int) ceil(((threshold * G1Item[0].size() * (G1Item[0].size() - 1)))/2) << endl;
		_threshold = (int) ceil(((threshold * G1Item[0].size() * (G1Item[0].size() - 1)))/2);
	}
	//PrintaVectorTS(G1Item);
	vector<BinaryMatrix *> vBM = constructBinaryMatrixAll(G1Item);
	/*for (int i = 0; i < (signed)vBM.size(); i++)
		{
			(*vBM[i]).PrintInfo();
			cout << endl;
		}*/
	BinaryMatrix BM = BinaryMatrix();
	cout << "Begin GLCM" << endl;
	//GLCMloop(P, BM, G1Item, vBM, freType);
	//PrintNoClosed();
	int G1ItemSized2 = G1Item.size()/2;
	for (int i = 0; i < G1ItemSized2; i++)
	{
		cout << "Item Sets start with: " << i + 1 << "+" << endl;
		ItemSet P1 = P.addItemwCore_i(i*2);
		BM = (*vBM[i*2]);
		//if ((i == 22) || (i == 40)) BM.PrintInfo();
		int resFre = BM.getSize();
		GLCMloop(P1, BM, G1Item, vBM, freType, so, output, resFre);
		//cout << "    -->> Found " << nbIS << " Item Sets." << endl;
	}
	/*BinaryMatrix tempBM = (*vBM[40]);//BinaryMatrix();
	tempBM &= (*vBM[22]);
	TransactionSequence tempTSS, tempTSS2;
	findMaxTranSeq(&tempBM, tempTSS);
	findMaxTranSeq(vBM[22], tempTSS2);
	cout << "max = " << tempTSS.size() << tempTSS2.size() << endl;
	bool bbb = false;
	if (tempBM == (*vBM[41]))
		bbb = true;//(tempBM == (*vBM[41]));
	cout << "bbb = " << bbb << endl;*/
	double ts1 = tt1.Stop();
	cout << endl << "Execution took " << setw(5) << ts1 << " sec" << endl;

	vector<BinaryMatrix*>::const_iterator posVBM ;
	for (posVBM = vBM.begin() ; posVBM != vBM.end() ; ++posVBM)
	  delete *posVBM ;
	  vBM.clear() ;

	ostringstream ost1;
	ost1 << getpid ();
	string cmd1="ps -p "+ost1.str ()+" -o rss";
	cout << "Execution Memory usage : " << endl;
	system(cmd1.c_str());
	cout << endl;

	//tt2.Start();
	//if (so == "y")
	//	write_to_file(freType);
	/*double ts2 = tt2.Stop();
	cout << endl << "Writing took " << setw(5) << ts2 << " sec" << endl;

	cout << "Memory usage at writing step: " << endl;
	system(cmd1.c_str());
	cout << endl;*/
}

void GLCM::write_to_file(string freType)//const char * filename
{
	TimeTracker tt2;
	tt2.Start();
	//print to file
	string st = "OUT/out";
	time_t tim=time(NULL);
	tm *now=localtime(&tim);
	stringstream ssm,ssd,ssh,ssmi,sss;//create a stringstream
	ssm << now->tm_mon + 1;//add number to the stream
	ssd << now->tm_mday;
	ssh << now->tm_hour;
	ssmi << now->tm_min;
	sss << now->tm_sec;
	st = st + ssm.str() + ssd.str() + ssh.str() + ssmi.str() + sss.str();

	char * filename = (char*) st.c_str();

	ofstream outf(filename);
	outf << ClosedItemSetList.size() <<" closed gradual frequent Item sets" << endl;

	list<ItemSet>::iterator it;
	//list<TransactionSequence>::iterator its =  MaxlengthTransactionSequenceList.begin ();
	//list<BinaryMatrix>::iterator itm =  BinaryMatrixList.begin ();
	for (it = ClosedItemSetList.begin (); it != ClosedItemSetList.end (); it++)
	{
		outf << "[ ";
		ItemSet itSet = *it;
		for (int i = 0 ; i < itSet.size() ; i++)
		{
			if ((itSet.getItemAt(i) % 2) == 0) outf << itSet.getItemAt(i)/2 +1 << "+ ";
			else outf << itSet.getItemAt(i)/2 +1 << "- ";
		}
		outf << "]" << endl;

		/*if (freType == "nor")
		{
			itm->PrintInfo();
			cout << endl;

			outf << " ---->>>> " << *its << endl << endl;
			its++;
			list<TransactionSequence> templTS = buildTSList(*itm);
			outf << " ---->>>> { ";
			list<TransactionSequence>::iterator templTSit;
			for (templTSit = templTS.begin (); templTSit != templTS.end (); templTSit++)
			{
				outf << * templTSit << " ";
			}
			outf << "}" << endl << endl;
			itm++;
		}
		else if (freType == "gra")
		{
			list<pair<int,int> > lPair = itm->findPair();
			outf << " ---->>>> { ";
			list<pair<int,int> >::iterator tempit;
			for (tempit = lPair.begin (); tempit != lPair.end (); tempit++)
			{
				outf << "(" << tempit->first << " " << tempit->second << ") ";
			}
			outf << "}" << endl << endl;
			itm++;
		}*/
	}

	outf.close();
	cout << "All Item sets have been written to file: " << st << endl;

	double ts2 = tt2.Stop();
	cout << endl << "Writing took " << setw(5) << ts2 << " sec" << endl;

	ostringstream ost1;
	ost1 << getpid ();
	string cmd1="ps -p "+ost1.str ()+" -o rss";
	cout << "Memory usage at writing step: " << endl;
	system(cmd1.c_str());
	cout << endl;
}

//check 1 int contains in a vector or not
bool GLCM::checkContains(int t, vector<int> vInt)
{
	for (int i = 0; i < (signed)vInt.size(); i++)
	{
		if (vInt[i] == t) return true;
	}
	return false;
}

void GLCM::buildTSListRecu(TransactionSequence& tTS, int transaction,BinaryMatrix BM, list<TransactionSequence>& tempLTS)
{
	tTS.addTransactionBack(transaction);
	vector<int> vChild = BM.findAllDirectChildren(transaction);
	if (vChild.size() == 0)
	{
		tempLTS.push_back(tTS);
		tTS.clear();
		return;
	}
	else
	{
		for(int i = 0; i < (signed)vChild.size(); i++)
		{
			buildTSListRecu(tTS,vChild[i],BM,tempLTS);
		}
	}
}

list<TransactionSequence> GLCM::buildTSList(BinaryMatrix BM)
{
	//list<pair<int,int> > lPair = BM.findPair();
	vector<int> vLO0 = BM.buildLeastOrder_0();
	list<TransactionSequence> tempLTS;
	tempLTS.clear();
	TransactionSequence tTS = TransactionSequence();
	for (int i = 0; i < (signed)vLO0.size(); i++)
	{
		int transaction = vLO0[i];
		tTS.clear();
		buildTSListRecu(tTS,transaction,BM,tempLTS);
	}
	return 	tempLTS;
	/*list<pair<int,int> > lPair = BM.findPair();
	list<pair<int,int> > lLOM = BM.buildLeastOrderMap();
	lLOM.sort(SortLOPair());
	list<TransactionSequence> tlTS;
	tlTS.clear();

	list<pair<int,int> >::iterator itlLOM = lLOM.begin();
	do {
		pair<int,int> pLOM = *itlLOM;
		//first add Transaction have no children
		if (pLOM.second == 0)
		{
			TransactionSequence tempTS = TransactionSequence(pLOM.first);
			tlTS.push_back(tempTS);
			//do next node
			itlLOM++;
		}
		else
		{
			//Add Transaction in the same group (have the same LeastOrder) to list
			//use another iterator
			pLOM = *itlLOM;
			int tempLO = pLOM.second;
			list<TransactionSequence> tempLTS;
			tempLTS.clear();
			do
			{
				//use a temp list of TransactionSequence to store

				//Pop all list elements
				list<TransactionSequence>::iterator tempit;
				for (tempit = tlTS.begin(); tempit != tlTS.end(); tempit++)
				{
					TransactionSequence itempTS = *tempit;
					bool iC = CompareTransaction(itempTS.getTransactionAt(itempTS.size() - 1),pLOM.first,lPair);
					// if last transaction greater than new Transaction (that need to be added) -> add new Transaction to Sequence and push it back to list
					if (iC)
						//add new Transaction to Transaction Sequence and push to a temp list
						itempTS.addTransactionBack(pLOM.first);

					if (!CheckListContains(itempTS,tempLTS))
					{
						tempLTS.push_back(itempTS);
					}
				}
				//cout << "Each STEP MAP DO" << tTM << endl;
				itlLOM++;
				pLOM = *itlLOM;
			}
			while (pLOM.second == tempLO);
			tlTS = tempLTS;
		}
			cout << "Each STEP LIST JOIN:";
			PrintaList(tlTS);
	}while(itlLOM != lLOM.end());
	return tlTS;*/
}

bool GLCM::CompareTransaction(int transaction1, int transaction2, list<pair<int,int> > lPair)
{
	if (lPair.size() != 0)
	{
		list<pair<int,int> >::iterator it;
		for (it = lPair.begin (); it != lPair.end (); it++)
		{
			pair<int,int> pair = *it;
			if ((pair.first == transaction1) && (pair.second== transaction2)) return true;
		}
	}
	return false;
}

bool GLCM::CheckListContains(TransactionSequence & P,list<TransactionSequence> & lTQ)
{
	list<TransactionSequence>::iterator itt;
	for (itt = lTQ.begin (); itt != lTQ.end (); itt++)
	{
		TransactionSequence ts = *itt;
		if(ts.contains(P)) return true;
	}
	return false;
}

void GLCM::write_TS_List_to_file(const char * filename)
{
	ofstream outf(filename);
	outf << MaxlengthTransactionSequenceList.size() <<" longest path" << endl; // Notice the space, it's important

	list<TransactionSequence>::iterator it;
	for (it = MaxlengthTransactionSequenceList.begin (); it != MaxlengthTransactionSequenceList.end (); it++)
	{
		outf << *it << " --> size: " << it->size() << endl;
	}
	outf.close();
}

void GLCM::PrintaVectorTS(vector<TransactionSequence> vTS)
{
	if (vTS.size()>0)
	{
		//list<TransactionSequence>::iterator it;
		for (int i = 0; i < (signed)vTS.size(); i++)
		{
			cout  << vTS[i] << endl;
		}
	}
}

void GLCM::PrintaListTS(list<TransactionSequence> lTS)
{
	list<TransactionSequence>::iterator it;
	for (it = lTS.begin (); it != lTS.end (); it++)
	{
		cout << *it << endl;
	}
}

void GLCM::Print_All_Closed_Pattern()
{
	list<ItemSet>::iterator it;
	cout << "Print all Closed Item set" << endl;
	for (it = ClosedItemSetList.begin (); it != ClosedItemSetList.end (); it++)
	{
		ItemSet itSet = *it;
		for (int i = 0 ; i < itSet.size() ; i++)
		{
			if ((itSet.getItemAt(i) % 2) == 0) cout << itSet.getItemAt(i)/2 +1 << "+ ";
			else cout << itSet.getItemAt(i)/2 +1 << "- ";
		}
		cout << endl;
	}
}

void GLCM::Print_An_Itemset(ItemSet IS)
{
	list<ItemSet>::iterator it;
	cout << " [";
	for (int i = 0; i < IS.size(); i++)
	{
		if ((IS.getItemAt(i) % 2) == 0) cout << IS.getItemAt(i)/2 +1 << "+";
					else cout << IS.getItemAt(i)/2 +1 << "-";
	}
	cout << "]" << endl;
}

void GLCM::PrintNoClosed()
{
	cout << "No of Closed Gradual Frequent Item Set: " <<  nbIS << endl;//ClosedItemSetList.size() << endl;
}


#endif /* __GLCM_CPP__ */
