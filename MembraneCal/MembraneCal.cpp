#include "DataStruct.h"
#include <queue>
Membrane *root;

string Str2Int(int id)
{
	if (id==0) return "0";
	string ret="";
	while (id)
	{
		ret=(char)(id%10+'0')+ret;
		id=id/10;
	}
	return ret;
}


/*****************************************************/
/****************Print********************************/
/*****************************************************/

#pragma region print
void printMembraneStruct()
{
	queue<Membrane*> q;
	q.push(root);
	while (!q.empty())
	{
		Membrane* node=q.front();
		q.pop();
		cout<<node->name<<": ";
		for(int i=0;i<node->sons.size();i++)
		{
			if (node->sons[i]->thick<=0) continue;
			cout<<node->sons[i]->name<<" ";
			q.push(node->sons[i]);
		}
		cout<<endl;
	}
}

void printMembraneContent()
{
	queue<Membrane*> q;
	q.push(root);
	while (!q.empty())
	{
		Membrane* node=q.front();
		q.pop();
		cout<<node->name<<": ";
		for(map<string,int>::iterator it=node->data.begin();it!=node->data.end();++it)
		{
			if (it->second>0){
				cout<<it->first<<"^"<<it->second<<" ";
			}
		}
		cout<<endl;
		for(int i=0;i<node->sons.size();i++)
		{
			if (node->sons[i]->thick<=0) continue;
			q.push(node->sons[i]);
		}
	}
}
int printNum=0;

void print()
{
	if (printNum==0)
		freopen("out.txt","w",stdout);
	cout<<"STEP:"<<printNum++<<" =====> "<<endl;
	cout<<"结构如下: "<<endl;
	printMembraneStruct();
	cout<<"膜内物质如下: "<<endl;
	printMembraneContent();
	cout<<endl<<endl;
//	fclose(stdout);
}


#pragma endregion print


#pragma region Cal

void del()
{
	for(int i=0;i<dels.size();i++){
		Membrane* node=dels[i];
		//物质转移
		for(map<string,int>::iterator it=node->data.begin();it!=node->data.end();it++)
		{
			if (node->parent->data[it->first]!=0) 	node->parent->data[it->first]+=it->second; else node->parent->data[it->first]=it->second;
		}
		Membrane * parent=node->parent;
		while(parent->parent!=NULL && parent->thick==0){parent=parent->parent;}
		//儿子转移
		for(int i=0;i<node->sons.size();i++)
		{
			parent->sons[node->parent->sons.size()-1]->next=node->sons[i];
			parent->sons.push_back(node->sons[i]);
			node->sons[i]->parent=parent;
		}
		//兄弟转移
		for(int i=0;i<node->parent->sons.size();i++)
		{
			if (node == parent->sons[i]){
				if (i!=0 && i+1<parent->sons.size()) parent->sons[i-1]->next=parent->sons[i+1];
				break;
			}
		}
	}
}

void copy()
{
	//同时子膜里面也有复制 怎么整- -！-------->从后往前就行了
	for(map<Membrane *,map<string,map<string,int>>>::reverse_iterator it=cpys.rbegin();it!=cpys.rend();++it)
	{
		for(map<string,map<string,int>>::iterator i=it->second.begin();i!=it->second.end();++i){
			Membrane* tmp = new Membrane();
		    Membrane * tmp2=tmp;
		    tmp->name=Str2Int(newId++);//i->first;
			Membrane* cur=it->first;
			queue<Membrane*> q;
			queue<Membrane*> tq;
			q.push(cur);
			tq.push(tmp);
			tmp->parent=cur->parent;
			tmp->state=cur->state;
			tmp->thick=cur->thick;
			tmp->type=cur->type;
			tmp->data=i->second;
			for(map<string,int>::iterator idd=cur->data.begin();idd!=cur->data.end();++idd)
				if (idd->second>0) tmp->data[idd->first]+=idd->second;
			tmp->rules=cur->rules;		
			int flag=0;
			while (!q.empty())
			{
				cur=q.front();q.pop();
				tmp=tq.front();tq.pop();
				if (flag){
					tmp->data=cur->data; 
					tmp->name=Str2Int(newId++); 
					tmp->rules=cur->rules;
					tmp->state=cur->state;
					tmp->thick=cur->thick;
					tmp->type=cur->type;
				}
				flag=1;
				for(int i=0;i<cur->sons.size();i++)
				{
					Membrane * tmp2=new Membrane();
					tmp->sons.push_back(tmp2);
					tmp2->parent=tmp;
					q.push(cur->sons[i]);
					tq.push(tmp2);
				}
			}
			it->first->parent->sons.push_back(tmp2);
		}
		it->first->thick=0;
	}
}

void in()
{
	for(vector<Move>::iterator it=ins.begin();it!=ins.end();it++)
	{
		if (it->to->thick>1) continue;//如果膜变厚
		for(map<string,int>::iterator itt=it->data.begin();itt!=it->data.end();itt++)
		{
			it->to->data[itt->first]+=itt->second;
		}
	}
}

void out(){
	for(vector<Move>::iterator it=outs.begin();it!=outs.end();it++)
	{
		for(map<string,int>::iterator itt=it->data.begin();itt!=it->data.end();itt++)
		{
			if(it->to!=NULL) it->to->data[itt->first]+=itt->second;
		}
	}
}

Membrane * getIn(Membrane * node)
{
	for(int i=0;i<node->sons.size();i++)
	{
		if (node->sons[i]->thick==1) return node->sons[i];
	}
	return node;//no son's thick==1 return itself 
}

int  Cal(Membrane* node ,Rule rule , map<string,int> &out)
{
	if ((node->thick)<=0) return 0;
	int p=0x3f3f3f3f;
	//in
 	for (map<string,int>::iterator it=rule.in.begin();it!=rule.in.end();++it)
	{
		if (node->data[it->first]<it->second) return 0;
		p=min(node->data[it->first]/it->second,p);
	}
	//condition
	for (map<string,int>::iterator it=rule.condition.begin();it!=rule.condition.end();++it)
	{
		if (node->data[it->first]<it->second) return 0;
	}
	for(int cnt=0;cnt<p;cnt++){
		for(map<string,int>::iterator it=rule.out.begin();it!=rule.out.end();++it)
		{
			if (it->first=="%") {  continue; }//spcial symbols, waiting to redo
			if (it->first=="$") {delThick[node]+=it->second; continue;}// 
			if (it->first=="&") {
				addThick[node]+=it->second; continue;}
			if (it->first=="in"){ 
				Move move; move.from=node;if (rule.inNum!=""){move.to=m[rule.inNum]; } else {move.to=getIn(node);} 
				move.data=rule.ins; ins.push_back(move); continue;}
			if (it->first=="out"){
				if (node==root) continue;
				Move move; move.from=node; move.to=node->parent; 
				move.data=rule.outs; outs.push_back(move); continue;}
			if (it->first=="copy"){
				for(map<string,map<string,int>>::iterator i=rule.cpys.begin();i!=rule.cpys.end();++i){
					cpys[node][i->first]=i->second;
				} 
				continue;}
			if(it->first=="son")
			{
				for(map<string,map<string,int>>::iterator i=rule.sons.begin();i!=rule.sons.end();++i){
				
					sons[node][i->first]=i->second;
			
				} 
				continue;
			}
			if (out[it->first]==0) out[it->first]=it->second ; else out[it->first]+=it->second ; //new product 
		}
	}
	for(map<string,int>::iterator it=rule.in.begin();it!=rule.in.end();++it)
	{
		node->data[it->first]-=it->second*p;
	}	
	return 1;
}

int Cal(Membrane* node)
{
	//the rules must be sorted by priovity before this method
	map<string,int> out;
	int ans=0;
	//cout<<node->thick<<endl;
	for(int i=0;i<node->rules.size();i++)
	{
		ans+=Cal(node,node->rules[i],out);
	}
	int ans2=0;
	for(map<string,int>::iterator it=out.begin();it!=out.end();++it){
		if (node->data[it->first]==0) node->data[it->first] = it->second; else node->data[it->first] += it->second;
		ans2+=it->second;
	}
	if (!ans && !ans2){node->state=0;}else{node->state=1;}
	return node->state;
}

int Cal()
{
	int ret=0;
	queue<Membrane*> q;
	q.push(root);
	while (!q.empty())
	{
		Membrane* node=q.front();
		q.pop();
	//	cout<<node->rules.size()<<endl;
		ret+=Cal(node);
		
		for(int i=0;i<node->sons.size();i++)
		{
			if (node->sons[i]->thick>0) q.push(node->sons[i]);
		}
	}
	return ret;
}
void thickChange()
{
	for(map<Membrane *,int>::iterator it=addThick.begin();it!=addThick.end();it++)
	{
		if (it->first->thick>0)	it->first->thick+=it->second;
	}	
	for(map<Membrane *,int>::iterator it=delThick.begin();it!=delThick.end();it++)
	{
		it->first->thick-=it->second;
		if (it->first->thick<=0){
			it->first->thick=0;
			dels.push_back(it->first);
		}
	}

	//if (delThick[i]->thick==0) dels.push_back(delThick[i]);}
	del();
}

void addson()
{
	for(map<Membrane *,	map<string ,map<string,int>> >::iterator it=sons.begin();it!=sons.end();++it)
	{
		for(map<string,map<string,int>>::iterator itt=it->second.begin();itt!=it->second.end();itt++){
			Membrane * tmp=new Membrane();
			string name=Str2Int(newId++);
			tmp->name=name;
			tmp->data=itt->second;
			tmp->parent=it->first;
			tmp->rules=it->first->rules;
		//	tmp->state=it->first->state;
			tmp->type=it->first->type;
			it->first->sons.push_back(tmp);
		
		}
		
	}

}
void CalStruct()
{
	in();
	out();
	addson();
	thickChange();
	copy();
}

void Run()
{
	dels.clear();	cpys.clear();	ins.clear(); outs.clear(); delThick.clear(); addThick.clear(); sons.clear();
	while (Cal()){ 
		CalStruct();
		print();
		dels.clear();	cpys.clear();	ins.clear(); outs.clear(); delThick.clear(); addThick.clear();sons.clear();};
}

#pragma endregion Cal




/*******************************************************/
/******Get Rules From the File**************************/
/*******************************************************/
string getname(string str)
{
	string ret;
	for(int i=0;i<str.size();i++){
		if (str[i]=='^') return ret;
		ret+=str[i];
	}
	return ret;
}
int getnum(string str)
{
	string ret;
	int i;
	for( i=0;i<str.size();i++)
	{
		if (str[i]=='^'){i++;break;}
	}
	if (i==str.size()) return 1;
	for(;i<str.size();i++)
	{
		ret+=str[i];
	}
	return atoi(ret.c_str());
}
int cmp(const Rule &a,const Rule &b)
{
	return a.priovity<b.priovity;
}
vector<char> ids;
int vis[26];
string getHaveId(string str)
{
	string ret="";
	int i;
	for(i=0;i<str.size();i++)
	{
		ret+=str[i];
		if(str[i]=='('){i++;break;}
	}
	if (i==str.size()) return ret;
	for(;i<str.size();i++)
	{
		if(str[i]<='z' && str[i]>='a') ret=ret+Str2Int(vis[str[i]-'a']); else ret+=str[i];
	}
	return ret;
}
void getRule(Rule rule,string name){
	Rule r;
	for(map<string,int>::iterator it=rule.in.begin();it!=rule.in.end();it++)
	{
		r.in[getHaveId(it->first)]=it->second;
	}
	for(map<string,int>::iterator it=rule.out.begin();it!=rule.out.end();it++)
	{
		r.out[getHaveId(it->first)]=it->second;
	}	
	for(map<string,int>::iterator it=rule.condition.begin();it!=rule.condition.end();it++)
	{
		r.condition[getHaveId(it->first)]=it->second;
	}
	for(map<string,int>::iterator it=rule.ins.begin();it!=rule.ins.end();it++)
	{
		r.ins[getHaveId(it->first)]=it->second;
	}
	for(map<string,int>::iterator it=rule.outs.begin();it!=rule.outs.end();it++)
	{
		r.outs[getHaveId(it->first)]=it->second;
	}
	for(map<string,map<string,int>>::iterator it=rule.cpys.begin();it!=rule.cpys.end();it++)
	{
		map<string,int> data;
		for(map<string,int>::iterator itt=it->second.begin();itt!=it->second.end();itt++)
		{
			data[getHaveId(itt->first)]=itt->second;
		}
		r.cpys[it->first]=data;
	}
	for(map<string,map<string,int>>::iterator it=rule.sons.begin();it!=rule.sons.end();it++)
	{
		map<string,int> data;
		for(map<string,int>::iterator itt=it->second.begin();itt!=it->second.end();itt++)
		{
			data[getHaveId(itt->first)]=itt->second;
		}
		r.sons[it->first]=data;
	}	
	r.priovity=rule.priovity;
	MembraneRules[name].push_back(r);
}
void dfs(int cur,int tot,Rule rule,string name){
	if(cur==tot){
		getRule(rule,name);
		return;
	}
	for(int i=0;i<MAXLABLE;i++)
	{
		vis[ids[cur]-'a']=i;
		dfs(cur+1,tot,rule,name);
	}
}
void AddRule(Rule rule,string name)
{
	memset(vis,0,sizeof(vis));
	dfs(0,ids.size(),rule,name);
}
void getId(string str)
{
	int i=0;
	for( i=0;i<str.size();i++)
	{
		if (str[i]=='(') {i++;break;}
	}
	if (i==str.size()) return;
	for(;i<str.size()-1;i++)
	{
		if(str[i]<='z' && str[i]>='a' && vis[str[i]-'a']==0){vis[str[i]-'a']=1;ids.push_back(str[i]);}
	}
}

int InOrOutRule(Rule &rule , string str)
{
	string tmp="";
	string type="";
	string id="";
	map<string,int> data;
	for(int i=1;i<str.size()-1;i++)
	{
		if (str[i]=='|' || str[i]==',')
		{
			getId(tmp);
			data[getname(tmp)]=getnum(tmp);
			tmp="";
		}else tmp=tmp+str[i];
		if (str[i]==','){
			for(int j=i+1;j<str.size()-1;j++,i++)
			{
				if (str[i]==')') continue;
				if (str[i]=='|')
				{
					for(int k=j+1;k<str.size()-1;i++,j++,k++)
					{
						if (str[k]==')') continue;
						id=id+str[k];
					}
				}else type=type+str[j];
			}
		}
	}
	if (type=="in")
	{
		rule.ins=data;
		if (id!=""){rule.inNum=id;}
		return 1;
	}else
	{
		rule.outs=data;
		if (id!=""){rule.outNum=id;}
		return 0;
	}
}
void CopyRule(Rule &rule,string str)
{
	string name=Str2Int(newId++);
	string tmp="";
	map<string,int> data;
	for(int i=1;i<str.size();i++)
	{
		if (str[i]=='|' || str[i]==']')
		{
			getId(tmp);
		    data[getname(tmp)]=getnum(tmp);
			tmp="";
		}else tmp+=str[i];
	}
	rule.cpys[name]=data;
}

void AddSonRule(Rule &rule,string str)
{
	string name=Str2Int(newId++);
	string tmp="";
	map<string,int> data;
	for(int i=1;i<str.size();i++)
	{
		if (str[i]=='|' || str[i]=='>')
		{
			getId(tmp);
		    data[getname(tmp)]=getnum(tmp);
			tmp="";
		}else tmp+=str[i];
	}
	rule.sons[name]=data;
}

void getRule()
{
	freopen("rule.txt","r",stdin);
//	ifstream file("rule.txt");
	string str;
	string name;
	int t=0;
	Rule rule;
	while (cin>>str && str!="@#")
	{
		Rule trule;
		if (str=="@"){
			cin>>name;
			t=0;
			continue;
		}
		if (str==","){t=t+1;continue;}
		if(t!=3 && str[0]!='(' && str[0]!='[') getId(str);
		switch (t){
			case 0:
				rule.in[getname(str)]=getnum(str);
				break;
			case 1:
				if(str[0] == '('){
					if (InOrOutRule(rule,str))
						rule.out["in"]=1;
					else 
						rule.out["out"]=1;
				}else if (str[0]=='['){
					CopyRule(rule,str);
					rule.out["copy"]=1;
				}else if (str[0]=='<'){
					AddSonRule(rule,str);
					rule.out["son"]=1;
				}else
					rule.out[getname(str)]=getnum(str);
				break;
			case 2:
				rule.condition[getname(str)]=getnum(str);
				break;
			case 3:
				rule.priovity=atoi(str.c_str());
				AddRule(rule,name);//MembraneRules[name].push_back(rule);
				memset(vis,0,sizeof(vis));
				ids.clear();
				t=0;
				rule=trule;
				break;
			default:
				break;
		}
	}
	//fclose(stdin);
	
	//freopen("CON","r",stdin);
	//sort the rules
	for(map<string,vector<Rule>>::iterator it=MembraneRules.begin();it!=MembraneRules.end();++it)
	{
		sort(it->second.begin(),it->second.end(),cmp);
	}
}
/*******************************************************/
/******Get Membrane Struct From the File****************/
/*******************************************************/
void getStruct()
{
//	freopen("struct.txt","r",stdin);

	ifstream file("struct.txt");
	string str;
	int f=0;
	while (getline(file,str))
	{
		string name;
		int i;
		for(i=0;i<str.size();i++)
		{
			if (str[i]==' ') continue;
			if (str[i]==':'){i+=2;break;}
			name+=str[i];
		}
		Membrane *parent;
		if (f==0){
			root->name=name;
			root->type=0;
			root->data=objects[name];
			root->rules=MembraneRules[name];
			parent=root;
			m[name]=root;
			f=1;
		}else{parent=m[name];}
		name="";
		Membrane * pre=NULL;
		for(;i<str.size();i++)
		{
			if (str[i]!=' ') name+=str[i];
			if (str[i]==' ' || (i==str.size()-1)){
				Membrane * node =new Membrane();
				node->name=name;
				node->data=objects[name];
				node->type=1;
				node->parent=parent;
				node->rules=MembraneRules[name];
				parent->sons.push_back(node);
				if (pre!=NULL){
					pre->next=node;
				}
				pre=node;
				m[name]=node;
				name="";
			}else name+=str[i];
		}
	}
	file.close();
	file.clear();
//	fclose(stdin);
	//freopen("CON","r",stdin);
	//fclose(stdin);
}

/*******************************************************/
/******Get Membrane Struct From the File****************/
/*******************************************************/
void getObject()
{
//	freopen("object.txt","r",stdin);
	ifstream file("object.txt");
	string str;
	//cin>>str;
	while (getline(file,str))
	{
		string name;
		int i;
		for( i=0;i<str.size();i++)
		{
			if (str[i]==' ') continue;
			if(str[i]==':') {i+=2;break;}//+=2 表示：后面有一个空格
			name+=str[i];
		}
		string content;
		for(;i<str.size();i++)
		{  if (str[i]!=' ') content+=str[i];
			if(str[i]==' ' || (i==str.size()-1)){
				objects[name][getname(content)]=getnum(content);
				content="";
			}
		}
	}
	file.close();
	file.clear();
	//fclose(stdin);
//	freopen("CON","r",stdin);
}
void init()
{
	getRule();
	getObject();
	getStruct();
}

int main()
{
	root=new Membrane();
	cout<<"开始读取文件..."<<endl;
	init();
	cout<<"文件读取完毕，开始计算..."<<endl;
	print();
	Run();
	fclose(stdout);
	freopen("CON","w",stdout);
	cout<<"计算完毕，生成输出文件..."<<endl;
	//print();
	cout<<"文件输出完毕"<<endl;
	freopen("CON","r",stdin);
	getchar();
	return 0;
}