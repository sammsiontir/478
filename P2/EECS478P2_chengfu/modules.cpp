#include "circuit.h"

int Circuit::createADDModule(const string &input1, const string &input2, const string &cin, const string &output, const string &cout, unsigned int numBits)
{
  Node* node;
  // create input/output nodes
  for (unsigned int i = 0; i < numBits; ++i){
    stringstream sstr;
    sstr << i;
    // input 1
    string name = input1 + "[" + sstr.str() + "]";
    node = createNode(name);
    // input 2
    name = input2 + "[" + sstr.str() + "]";
    node = createNode(name);
    // output
    name = output + "[" + sstr.str() + "]";
    node = createNode(name);

    // temp node for calculation
    name = output + "_AB[" + sstr.str() + "]";
    node = createNode(name);
    name = output + "_AxorB[" + sstr.str() + "]";
    node = createNode(name);
    name = output + "_ciAxorB[" + sstr.str() + "]";
    node = createNode(name);
  }

  // create cin nodes
  node = createNode(cin);
  // create c node
  for (unsigned int i = 0; i < numBits-1; ++i){
    stringstream sstr;
    sstr << i;
    string name = output + "_c[" + sstr.str() + "]";
    node = createNode(name);
  }
  // create cout nodes
  node = createNode(cout);

  // create zero
  Node* zero_node = createNode("zero");
  createZERONode(zero_node);

  for (unsigned int i = 0; i < numBits; ++i)
  {
    string name;
    stringstream inStr;
    stringstream inStr_prev;
    inStr << i;
    inStr_prev << i-1;
    // find input A[i]: input1[i]
    name = input1 + "[" + inStr.str() + "]";
    Node* inNode1 = findNode(name);
    assert(inNode1 != NULL);
    // find input B[i]: input2[i]
    name = input2 + "[" + inStr.str() + "]";
    Node* inNode2 = findNode(name);
    assert(inNode2 != NULL);
    // find output S[i]: output[i]
    name = output + "[" + inStr.str() + "]";
    Node* outNode = findNode(name);
    assert(outNode != NULL);

    // find ci
    if(i==0) name = cin;
    else name = output + "_c[" + inStr_prev.str() + "]";
    Node* ciNode = findNode(name);
    assert(ciNode != NULL);
    // find co
    if(i==numBits-1) name = cout;
    else name = output + "_c[" + inStr.str() + "]";
    Node* coNode = findNode(name);
    assert(coNode != NULL);

    // com[pute output
    // S[i]=A[i] xor B[i] xor cin[i]
    createXOR3Node(inNode1, inNode2, ciNode, outNode);

    // conmpute cout
    // cout[i] = A[i] and B[i] or cin[i] and A[i] xor B[i]
    // A[i] and B[i]
    name = output + "_AB[" + inStr.str() + "]";
    Node* AB = findNode(name);
    assert(inNode1 != NULL);
    createAND2Node(inNode1, inNode2, AB);
    // A[i] xor B[i]
    name = output + "_AxorB[" + inStr.str() + "]";
    Node* AxorB = findNode(name);
    assert(inNode1 != NULL);
    createXOR3Node(zero_node, inNode1, inNode2, AxorB);
    // ci[i] and A[i] xor B[i]
    name = output + "_ciAxorB[" + inStr.str() + "]";
    Node* ciAxorB = findNode(name);
    assert(inNode1 != NULL);
    createAND2Node(ciNode, AxorB, ciAxorB);
    // AB or ciAxorB
    createOR2Node(AB, ciAxorB, coNode);

  }

  return 0;
}

int Circuit::createSUBModule(const string &input1, const string &input2, const string &output, unsigned int numBits)
{
  Node* node;
  // create input/output nodes
  for (unsigned int i = 0; i < numBits; ++i){
    stringstream sstr;
    sstr << i;
    // input 1
    string name = input1 + "[" + sstr.str() + "]";
    node = createNode(name);
    // input 2
    name = input2 + "[" + sstr.str() + "]";
    node = createNode(name);
    // output
    name = output + "[" + sstr.str() + "]";
    node = createNode(name);
    // borrow
    name = output + "_Bo[" + sstr.str() + "]";
    node = createNode(name);

    // temp node for calculation
    name = output + "_AxorB[" + sstr.str() + "]";
    node = createNode(name);
    name = output + "_notAxorB[" + sstr.str() + "]";
    node = createNode(name);
    name = output + "_temp1[" + sstr.str() + "]";
    node = createNode(name);
    name = output + "_notA[" + sstr.str() + "]";
    node = createNode(name);
    name = output + "_temp2[" + sstr.str() + "]";
    node = createNode(name);
  }

  Node* zero_node = createNode("zero");
  createZERONode(zero_node);
  Node* one_node = createNode("one");
  createONENode(one_node);

  for (unsigned int i = 0; i < numBits; ++i)
  {
    string name;
    stringstream inStr;
    stringstream inStr_prev;
    inStr << i;
    inStr_prev << i-1;
    // find input A[i]: input1[i]
    name = input1 + "[" + inStr.str() + "]";
    Node* inNode1 = findNode(name);
    assert(inNode1 != NULL);
    // find input B[i]: input2[i]
    name = input2 + "[" + inStr.str() + "]";
    Node* inNode2 = findNode(name);
    assert(inNode2 != NULL);
    // find output S[i]: output[i]
    name = output + "[" + inStr.str() + "]";
    Node* outNode = findNode(name);
    assert(outNode != NULL);

    // find previous Borrow
    if(i==0) name = "zero";
    else name = output + "_Bo[" + inStr_prev.str() + "]";
    Node* PBoNode = findNode(name);
    assert(PBoNode != NULL);

    // find Borrow
    name = output + "_Bo[" + inStr.str() + "]";
    Node* BoNode = findNode(name);
    assert(BoNode != NULL);

    // com[pute output
    // S[i]=A[i] xor B[i] xor PBo[i]
    createXOR3Node(inNode1, inNode2, PBoNode, outNode);

    // conmpute Borrow
    // Bo[i] = PBo and not(A[i] xor B[i]) and not(A[i]) and B[i]
    // A[i] xor B[i]
    name = output + "_AxorB[" + inStr.str() + "]";
    Node* AxorB = findNode(name);
    assert(AxorB != NULL);
    createXOR3Node(zero_node, inNode1, inNode2, AxorB);
    // not(AxorB)
    name = output + "_notAxorB[" + inStr.str() + "]";
    Node* notAxorB = findNode(name);
    assert(notAxorB != NULL);
    createXOR3Node(zero_node, AxorB, one_node, notAxorB);
    // PBo and not(AxorB)
    name = output + "_temp1[" + inStr.str() + "]";
    Node* temp1 = findNode(name);
    assert(temp1 != NULL);
    createAND2Node(PBoNode, notAxorB, temp1);
    // not(A)
    name = output + "_notA[" + inStr.str() + "]";
    Node* notA = findNode(name);
    assert(notA != NULL);
    createXOR3Node(zero_node, inNode1, one_node, notA);
    // not(A) and B
    name = output + "_temp2[" + inStr.str() + "]";
    Node* temp2 = findNode(name);
    assert(temp2 != NULL);
    createAND2Node(notA, inNode2, temp2);
    // Bo
    createOR2Node(temp1, temp2, BoNode);
  }

  // when you have implemented this function,
  // change 'return -1' to 'return 0'
  return 0;
}


int Circuit::createSHIFTModule(const string &input, const string &output, unsigned int numBits, unsigned int numShift)
{
  Node* node;
  // create input nodes
  for (unsigned int i = 0; i < numBits; ++i)
  {
    stringstream sstr;
    sstr << i;
    string name = input + "[" + sstr.str() + "]";
    
    node = createNode(name);
  }
  
  // create output nodes
  for (unsigned int i = 0; i < numBits+numShift; ++i)
  {
    stringstream sstr;
    sstr << i;
    string name = output + "[" + sstr.str() + "]";
    
    node = createNode(name);
  }
  
  // assign '0's to the least numShift bits
  Node* zeroNode = createNode("ZERO");
  createZERONode(zeroNode);
  
  for (unsigned int i = 0; i < numShift; ++i)
  {
    stringstream sstr;
    sstr << i;
    string name = output + "[" + sstr.str() + "]";
    
    Node* outNode = findNode(name);
    assert(outNode != NULL);
    
    createBUF1Node(zeroNode, outNode);
  }
  
  // assign inputs to the remaining numBits bits
  for (unsigned int i = numShift; i < numBits+numShift; ++i)
  {
    string name;
    
    // find input node[i-numShift]
    stringstream inStr;
    inStr << i-numShift;
    name = input + "[" + inStr.str() + "]";
    Node* inNode = findNode(name);
    assert(inNode != NULL);
    
    // find output node[i]
    stringstream outStr;
    outStr << i;
    name = output + "[" + outStr.str() + "]";
    Node* outNode = findNode(name);
    assert(outNode != NULL);
    
    // assign
    createBUF1Node(inNode, outNode);
  }
  
  return 0;
}

