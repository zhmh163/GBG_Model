#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <stdio.h>
#include <algorithm>    // std::find
#include <vector>       // std::vector
#include <set>			//std::set
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include <sstream>
#include <utility>
using namespace std;
#include "Randnum_MT.h"


long int N;         //Node number 
#define pi          3.141592653589793238462643383279502884197   
double rand_num;
MTRand Rnd(1973);// open a class with automatically generate seed(1977)
//////////////////////////////////////////////////////////////////////////
vector<vector<long int> > edge_list;
vector<double> theta;
vector<double> kappa;

vector<vector<long int> > Read_Graph(string filename) //should check the file name.
{
	long int i,j;
	//filename+="_edgelist.txt";
	ifstream infile(filename.c_str()); 
	N=-1;
	while (infile >> i)
	{
		if(i>N)
		{
			N=i;
		}
	}
	N=N+1;//max node number// id+1
	vector<vector<long int> > adj_list(N);	
	infile.clear();
    infile.seekg(0, std::ios_base::beg);
	while (infile >> i >> j)
	{
		if ( (i!=j) && (find(adj_list[i].begin(), adj_list[i].end(), j) == adj_list[i].end()) )// can't self and multi-connection!
		{
			adj_list[i].push_back(j);	
			adj_list[j].push_back(i);	
		}
	}
	infile.close();	
	cout<<"input adj file is success!"<<endl;
	cout<<"total number:"<<N<<endl;
	return adj_list;
}
void Read_parameters(string filename, long int &realN, double &beta, double & mu) //should check the file name.
{
	ifstream infile(filename.c_str());	
	infile>>realN>>beta>>mu;	
	cout<<"read Nodes Bet Mu success!"<<endl;
	cout<<"read Nodes "<<realN<<endl;
	cout<<"beta "<<beta<<endl;
	cout<<"mu "<<mu<<endl;
	infile.close();

}
void Read_kappa_theta(string filename, vector<double> &kappa, vector<double> &theta, long int Nn) //should check the file name.
{
	// should include ....#include <sstream> #include <string> 
	//initialization kappa, theta;
	vector<double> kappa0(Nn,0.0);
	vector<double> theta0(Nn,0.0);
	kappa.swap(kappa0);
	theta.swap(theta0);
	
	//filename+="_coordinates.txt";
	string line;
	ifstream file(filename.c_str());
	while (getline(file, line))
	{
		if (line[0] != '#' ) //without comment
		{
			std::istringstream iss(line);
			long int i;
			double kappai,thetai;
			while(iss >> i >> kappai>> thetai)
			{
				kappa[i]=kappai;
				theta[i]=thetai;
			}
		}
	}
	file.close();
	cout<<"read kappa theta success!"<<endl;
}
vector<double> Read_kappa_from_stable_dist(string filename, vector<long int> &node_in_which_supernode, vector<vector<long int> > &supernode_contain_which_node,long int &N, long int &Nl1) //should check the file name.
{
	long int i,j;
	filename+=".txt";
	ifstream infile(filename.c_str()); 	
	long int node;
	long int supernode;
	double kappai;
	//find the number of subnode and supernode	
	N=-1;
	Nl1=-1;	
	while(infile >> node>> supernode>> kappai)			
	{
		if(node>Nl1)
		{
			Nl1=node;
		}
		if(supernode>N)
		{
			N=supernode;
		}
	}
	N=N+1;//max node number// id+1
	Nl1=Nl1+1;	
	cout<<"total number of supernodes:"<<N<<endl;
	cout<<"total number of nodes in uplayer:"<<Nl1<<endl;
	
	
	//node i in wich supernode 
	//initialization node_in_which_supernode(N, -1);
	vector<long int> tempvector(Nl1, -1);
	node_in_which_supernode.swap(tempvector);
	//initialization supernode_contain_which_node(Nn)
	vector<vector<long int> > tvector(N); //supernode[i] contain sub node j1--j2--j3......
	supernode_contain_which_node.swap(tvector);
	
	vector<double> kappa_l1(Nl1,0.0);
	
	infile.clear();
    infile.seekg(0, std::ios_base::beg);
	while(infile >> node>> supernode>> kappai)	
	{		
		node_in_which_supernode[node]=supernode;
		supernode_contain_which_node[supernode].push_back(node);
		kappa_l1[node]=kappai;
	}
	infile.close();		
	return kappa_l1;
}
void sort_index(vector<vector<long int> > &edge_list, vector<double> &kappa, vector<double> &theta, long int &N) //re-asign the id for each node
{
    // Declaring vector of pairs
    vector< pair <double, long int> > vect;
 
    // Initializing 1st and 2nd element of pairs of theta
    // Entering values in vector of pairs
    for (long int i=0; i<N; i++)
	{
		if(edge_list[i].size()>0)
		{
			vect.push_back( make_pair(theta[i], i) );
		}
	}  

    // Using simple sort() function to sort
    sort(vect.begin(), vect.end());
 
     // Printing the sorted vector(after using sort())
    vector<double> new_theta;
	vector<double> new_kappa;
	vector<long int> new_id(N,0);
    for (long int i=0; i<vect.size(); i++)
    {
        // vect[i].first-->theta, vect[i].second-->old index
        // 1st and 2nd element of pair respectively
		new_theta.push_back(vect[i].first);
		new_kappa.push_back( kappa[vect[i].second] );
        new_id[vect[i].second]=i;
    } 
	new_theta.swap(theta);
	new_kappa.swap(kappa);
	vector<vector<long int> > new_edge_list(vect.size());
	for(long int i=0;i<N;i++)
	{
		for(long int j=0;j<edge_list[i].size();j++)
		{
			long int a=new_id[i];
			long int b=new_id[edge_list[i][j]];
			new_edge_list[a].push_back(b);
		}
	
	}
	new_edge_list.swap(edge_list);
	N=vect.size();
	//cout<<"N is"<<N<<endl;
   // return new_id;
}
int Real_N(vector<vector<long int> > edge_list, long int Nn)
{
	
	long int realN=0;	
	for(long int i=0;i<Nn;i++)
	{		
		if (edge_list[i].size() > 0)
		{
			realN++;
		}
		
	}
	return realN;
}
double conncted_probability(double theta1,double theta2,double kappa1,double kappa2,long int Nn, double beta, double mu)
{
	double delta_theta=pi-abs(pi-abs(theta1-theta2));			
	double d=(double)Nn/(2*pi)*delta_theta;
	double chi=1.0/(1.0+pow(d/(mu*kappa1*kappa2), beta) );	
	return chi;
}
vector<double> cal_connect_pattern(vector<double> p)
{
	long int pattern_size=(int)(p.size());
	vector<double> P(pattern_size,0.0);	
    double prb=1.0;
	for(long int i=0;i<p.size();i++)
	{
		prb*=(1-p[i]);
	}
	for(long int i=0;i<p.size();i++)
	{
		rand_num=Rnd.randExc();
		if(rand_num<p[i]/(1-prb))
		{
			P[i]=1;
			for(long int j=i+1;j<p.size();j++)
			{
				double randx;
				randx=Rnd.randExc();
				if(randx<p[j])
				{
					P[j]=1;			
				}

			}
			break;
		}
		else
		{
			prb=prb/(1-p[i]);
		}
	}
	/*for(long int i=1;i<P.size();i++)
	{
		cout<<P[i];	
	}
	cout<<endl;*/
	return P;
}
double correct_theta(double xtheta)
{
	while(xtheta>2*pi)
	{
		xtheta=xtheta-2*pi;	
	}
	while(xtheta<0)
	{
		xtheta=xtheta+2*pi;	
	}
	return xtheta;
}
void meank_maxk(vector<vector<long int> > edge_list, long int Nn, double &meank, double &k_with_degree0)
{
	meank=0;
	k_with_degree0=0;
	long int maxk=0;
	double sumn = 0;
	for(long int i=0;i<Nn;i++)
	{
		if(edge_list[i].size()>maxk)
			maxk=edge_list[i].size();
		if (edge_list[i].size() > 0)
			sumn++;
		meank+=edge_list[i].size();
	}
	k_with_degree0=meank/(double) (Nn);
	meank /= sumn;	
	//cout<<"maxk="<<maxk<<" "<<setw(16)<<"mean_degree="<<meank<<endl;
	
}

vector<vector<long int> > Growth(vector<vector<long int> > edge_list, long int Nn, long int Nl1, vector<double> &kappa, vector<double> &theta, vector<double> kappa_l1,\
		vector<long int> &node_in_which_supernode, vector<vector<long int> > &supernode_contain_which_node, double r, double beta, double mu, double mu_new,double alpha)
{	
	vector<double> theta_l1(Nl1, 0.0);	
	vector<vector<long int> > aij(Nl1);  //aij edge list in layer 1	
	//****************************************************************************************	
	//asign theta to sub nodes 	
	for(long int i=0;i<Nn;i++)
	{	
		if(supernode_contain_which_node[i].size()==1) //i do not split
		{
			long int j=supernode_contain_which_node[i][0];
			theta_l1[j]=theta[i];
		}
		else //split 2
		{
			long int j1,j2;
			//make sure j1 in the left, and j2 in the right of i
			if(supernode_contain_which_node[i][0]<supernode_contain_which_node[i][1])
			{
				j1=supernode_contain_which_node[i][0];
				j2=supernode_contain_which_node[i][1];
			}
			else
			{
				j1=supernode_contain_which_node[i][1];
				j2=supernode_contain_which_node[i][0];
			}			
			//find j1's theta			
			double dtheta;
			double dx_super;
			dtheta=2*pi/(double)(Nl1);
			if(i==0)
			{
				dx_super=pi-abs(pi-abs(theta[i]-theta[Nn-1]));
			}
			else
			{
				dx_super=pi-abs(pi-abs(theta[i]-theta[i-1]));
			}
			if(dtheta>dx_super/2.0)
			{
				dtheta=dx_super/2.0;			
			}			
			theta_l1[j1]=theta[i]-dtheta*Rnd.randDblExc();		//rand_num between (theta[i]-dtheta, theta[i])
			theta_l1[j1]=correct_theta(theta_l1[j1]);
			// find j2's theta
			dtheta=2*pi/(double)(Nl1);
			if(i==Nn-1)
			{
				dx_super=pi-abs(pi-abs(theta[0]-theta[i]));
			}
			else
			{
				dx_super=pi-abs(pi-abs(theta[i]-theta[i+1]));
			}
			if(dtheta>dx_super/2.0)
			{
				dtheta=dx_super/2.0;			
			}			
			theta_l1[j2]=theta[i]+dtheta*Rnd.randDblExc();		//rand_num between (theta[i], dtheta+theta[i])
			theta_l1[j2]=correct_theta(theta_l1[j2]);			
		}
	}
	//cout<<"finished theta part!!"<<endl;getchar();
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
	// connectied edges between sub nodes
	//update mu

	for(long int i=0;i<Nl1;i++)
	{
		aij[i].clear();
	}
	long int supernode1, supernode2;
	//connected sub nodes in the same supernode 
	for(supernode1=0;supernode1<Nn;supernode1++)
	{	
		if(supernode_contain_which_node[supernode1].size()>1)
		{
			long int a1, a2;
			a1=supernode_contain_which_node[supernode1][0];
			a2=supernode_contain_which_node[supernode1][1];

			double chi=conncted_probability(theta_l1[a1],theta_l1[a2],kappa_l1[a1],kappa_l1[a2], Nl1, beta, mu);	
			rand_num=Rnd.randExc();  // real number in [0,1)
			if(rand_num<chi && (find(aij[a1].begin(), aij[a1].end(), a2) == aij[a1].end()))
			{
				aij[a1].push_back(a2);
				aij[a2].push_back(a1);
			}
		}
	}	
	//connected sub nodes between two super nodes 
	for(supernode1=0;supernode1<Nn;supernode1++)
	{		
		for(long int k=0;k<edge_list[supernode1].size();k++)
		{
			supernode2=edge_list[supernode1][k];
			if(supernode2>supernode1)// up triangle
			{
				long int i,j;
				vector <double> pr;
				pr.clear();
				long int two_points_in_a_link[4][2];
				long int m=0;				
				for(long int e1=0;e1<supernode_contain_which_node[supernode1].size();e1++)
				{	
					i=supernode_contain_which_node[supernode1][e1];
					for(long int e2=0;e2<supernode_contain_which_node[supernode2].size();e2++)
					{						
						j=supernode_contain_which_node[supernode2][e2];										
						double chi=conncted_probability(theta_l1[i],theta_l1[j],kappa_l1[i],kappa_l1[j], Nl1, beta, mu);	
						pr.push_back(chi);
						two_points_in_a_link[m][0]=i;
						two_points_in_a_link[m][1]=j;
						m++;
					}
				}
				//cal the connection pattern between four nodes.
				vector<double> Pn;
				Pn=cal_connect_pattern(pr);
				for(long int n=0;n<Pn.size();n++)
				{					
					if(Pn[n]==1)
					{
						long int i=two_points_in_a_link[n][0];
						long int j=two_points_in_a_link[n][1];
						if( find(aij[i].begin(), aij[i].end(), j) == aij[i].end() )
						{
							aij[i].push_back(j);
							aij[j].push_back(i);							
						}
					}
				}


			}	
		}
	}
   //add extral links
	vector<vector<long int> > aij_update(Nl1);  //aij edge list in layer 1	
//	double mu_old=mu;	
	//mu=pow(alpha,layer)*mu_old;
//	mu=alpha*mu_old;
	//add links between i j with probability chi
	aij_update=aij;
	if(alpha>1)
	{
		for(long int i=0;i<Nl1;i++)
		{
			for(long int j=i+1;j<Nl1;j++)
			{
				if(find(aij_update[i].begin(), aij_update[i].end(), j) == aij_update[i].end())
				{
					double p_new=conncted_probability(theta_l1[i],theta_l1[j],kappa_l1[i],kappa_l1[j], Nl1, beta, mu_new);	
					double p_l=conncted_probability(theta_l1[i],theta_l1[j],kappa_l1[i],kappa_l1[j], Nl1, beta, mu);
					double chi=(p_new-p_l)/(1-p_l);
					rand_num=Rnd.randExc();  // real number in [0,1)
					if(rand_num<chi)
					{
						aij_update[i].push_back(j);
						aij_update[j].push_back(i);
					}
				}

			}

		}	
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//reture kappa, theta and node_in_which_supernode in layer 1;
	kappa.swap(kappa_l1);
	theta.swap(theta_l1);

	return aij_update;
}
vector<double > cluster_coefficence(vector<vector<long int> > edge_list, long int Nn, double &meanC) //The clustering coefficient of each node
{
	long int i,j,k,u,v;
	vector<double > Cluster_coeff;
	double C,ci;	
	C=0.0;
	double sum=0; //sum is the number of degree larger than 1
	for(i=0;i<Nn;i++)
	{
		ci=0.0;
		if(edge_list[i].size()>1)//degree larger than 1
		{
			for(j=0;j<edge_list[i].size();j++)
			{
				for(k=j+1;k<edge_list[i].size();k++)
				{
					u=edge_list[i][j];
					v=edge_list[i][k];
					if(find(edge_list[u].begin(),edge_list[u].end(),v) !=edge_list[u].end())
					{
						ci+=1.0;
					}
				}
			}		
			ci=2.0*ci/(edge_list[i].size()* (edge_list[i].size()-1.0));
			sum++;
		}
		else
			ci=0.0;
		Cluster_coeff.push_back(ci);
		C+=ci;
	}
	//C/=(double)(Nn);
	C/=sum; //sum is the number of degree larger than 1
	meanC=C;
	//cout<<"the mean clustering coefficient is:"<<" "<<setw(16)<<meanC<<endl;
	return Cluster_coeff;
}
void output_edgelist(string filename, vector<vector<long int> > edge_list,long int Nn)
{
	string file1,file2;
	file1=filename+"_edgelist.txt";	
	file2=filename+"_coordinates.txt";	
	ofstream outfile10(file1.c_str(),ios::out); 	
	ofstream outfile11(file2.c_str(),ios::out); 
	
	for(long int i=0;i<Nn;i++)
	{
		for(long int k=0;k<edge_list[i].size();k++)
		{
			if(edge_list[i][k]>i)
				outfile10<<setprecision(10)<<left<<setw(16)<<i<<" "<<setw(16)<<edge_list[i][k]<<endl;		
		}
		if(edge_list[i].size()>0)
		outfile11<<setprecision(10)<<left<<setw(16)<<i<<" "<<setw(16)<<kappa[i]<<" "<<setw(16)<<theta[i]<<endl;
	}
	outfile10.close();
	outfile11.close();
}
void output_supernode(string filename, vector<long int>  node_in_which_supernode)
{
	string file1;
	file1=filename+"_supernode.txt";		 
	ofstream outfile10(file1.c_str(),ios::out); 	
	 
	for(long int i=0;i<node_in_which_supernode.size();i++)
	{		
		outfile10<<left<<" "<<setw(16)<<i<<" "<<setw(16)<<node_in_which_supernode[i]<<endl;
	}
	outfile10.close();	
}

double IRG_Max_me_Citation(double alpha,string filepath, int total_layer)
{
	double time_begin=time(NULL);
	double r;
	ostringstream fout1;
	fout1 <<filepath<<"_Maxme_growth_layer_meanC_meank_N.txt";
	string fout_C_K_N= fout1.str();			
	ofstream outfile10(fout_C_K_N,ios::out);	
	double meanC=0.0;
	double meanK=0.0;
	double k_with_degree0=0;
	long int realN;			
	double beta, mu0, mu, mu_new;

	// input parameters, edgelist, coordinates from file
	ostringstream fin1;
	fin1 <<filepath<<"_parameters.txt";
	string f_para= fin1.str();		
	Read_parameters(f_para, realN, beta, mu0);

	ostringstream fin2;
	fin2 <<filepath<<"_edgelist.txt";
	string f_edge= fin2.str();		
	edge_list=Read_Graph(f_edge);

	ostringstream fin3;
	fin3 <<filepath<<"_coordinates.txt";
	string f_coord= fin3.str();			
	Read_kappa_theta(f_coord, kappa, theta, N);
	//***********************************************


	vector<double > Cc;
	Cc=cluster_coefficence(edge_list,N,meanC);
	meank_maxk(edge_list, N, meanK, k_with_degree0); //return meank_no_isolated, meank_with_isolated_node.


	int layer=0;
	mu_new=mu0;
	mu=mu0;
	realN=Real_N(edge_list,N);
	cout<<" "<<left<<setw(16)<<layer<<" "<<setw(16)<<meanC<<" "<<setw(16)<<meanK<<" "<<setw(16)<<k_with_degree0<<" "<<setw(16)<<realN<<" "<<setw(16)<<N<<" "<<setw(16)<<mu<<" "<<setw(16)<<mu_new<<endl;
	//outfile10<<" "<<left<<setw(16)<<"# layer"<<" "<<setw(16)<<"meanC"<<" "<<setw(16)<<"meanK"<<" "<<setw(16)<<"k_with_degree0"<<" "<<setw(16)<<"realN"<<" "<<setw(16)<<"N_with_degree0"<<" "<<setw(16)<<"mu"<<" "<<setw(16)<<"mu_new"<<endl;
	
	outfile10<<" "<<left<<setw(16)<<layer<<" "<<setw(16)<<meanC<<" "<<setw(16)<<meanK<<" "<<setw(16)<<k_with_degree0<<" "<<setw(16)<<realN<<" "<<setw(16)<<N<<" "<<setw(16)<<mu<<" "<<setw(16)<<mu_new<<endl;


	ostringstream fout2;
	fout2 <<filepath<<"_Maxme_growth_layer_"<<layer;
	string fout_edge = fout2.str();			
	output_edgelist(fout_edge, edge_list, N);

	//IRG process begin*************

	for(int layer=1;layer<=8;layer++)
	{			
		vector<long int> node_in_which_supernode;	
		vector<vector<long int> > supernode_contain_which_node;
		vector<double> kappa_l1;
		long int N;
		long int Nl1;			
		//change the file name of kappa from stable distribution
		ostringstream file1;
		file1 <<filepath<<"_kappa_l_"<<layer;
		string File_name_kappa = file1.str();	
		kappa_l1=Read_kappa_from_stable_dist(File_name_kappa, node_in_which_supernode, supernode_contain_which_node, N, Nl1);
		//****return kappa_l1, node_in_which_supernode, supernode_contain_which_node, N, Nl1****

		//get the edge list in the up layer****
		//update mu		
		//r will be change as Nl1; meank_0 in uplayer can be calculated using the fitting parameters; 
		r=(double)(Nl1)/(double)(N);			
		mu=r*mu_new;
		mu_new=alpha*mu;
		edge_list=Growth(edge_list, N, Nl1, kappa, theta, kappa_l1, node_in_which_supernode, supernode_contain_which_node,r, beta, mu, mu_new, alpha);
		//return edgelist,kappa, theta, mu_new in uplayer  	

		//cal mean C, mean degree and real N	
		Cc=cluster_coefficence(edge_list, Nl1, meanC);
		meank_maxk(edge_list, Nl1, meanK, k_with_degree0);
		realN=Real_N(edge_list,Nl1);

		//output ave. C K N.....
		cout<<" "<<left<<setw(16)<<layer<<" "<<setw(16)<<meanC<<" "<<setw(16)<<meanK<<" "<<setw(16)<<k_with_degree0<<" "<<setw(16)<<realN<<" "<<setw(16)<<Nl1<<" "<<setw(16)<<mu<<" "<<setw(16)<<mu_new<<endl;
		outfile10<<" "<<left<<setw(16)<<layer<<" "<<setw(16)<<meanC<<" "<<setw(16)<<meanK<<" "<<setw(16)<<k_with_degree0<<" "<<setw(16)<<realN<<" "<<setw(16)<<Nl1<<" "<<setw(16)<<mu<<" "<<setw(16)<<mu_new<<endl;

		//output the edgelist of uplayer
		ostringstream fileNameStream;
		fileNameStream <<filepath<<"_Maxme_growth_layer_"<<layer;
		string fileName = fileNameStream.str();			
		output_edgelist(fileName, edge_list, Nl1);


		ostringstream fout3;
		fout3 <<filepath<<"_Maxme_"<<layer;
		string f_super = fout3.str();			
		output_supernode(f_super, node_in_which_supernode);
	}
	outfile10.close();
		

	cout<<"finish calculating!"<<endl;
	double time_end=time(NULL);
    cout<<"RUN TIME: "<<time_end-time_begin<<endl;		
	return time_end-time_begin;
}
int main(int argc, char *argv[])
{	
	string filepath=argv[1];	// filepath to load related file from stable distribution
	double alpha=stod(argv[2]);  // a to adjust average degree		
	int total_layer=stoi(argv[3]); //total_layer	
	double run_time;
	run_time=IRG_Max_me_Citation(alpha, filepath, total_layer);
	return 0;
}



