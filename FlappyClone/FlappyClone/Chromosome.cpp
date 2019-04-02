#include "Chromosome.h"

Chromosome::Chromosome() 
{
}

Chromosome::~Chromosome() 
{
}

void Chromosome::addGene(Gene gene)
{
	m_genes.push_back(gene);
}

std::vector<Gene> & Chromosome::getGenes()
{
	return m_genes;
}

void Chromosome::print()
{
	for (auto & gene : m_genes)
	{
		std::cout << "From Id: " << gene.fromId << ", To Id: " << gene.toId << ", Weight: " << gene.weight << std::endl;
	}
}

void Chromosome::saveToTextFile(std::string path)
{
	std::ofstream fileStream;
	fileStream.open(path, std::ofstream::out | std::ofstream::app);
	for (auto & gene : m_genes)
	{
		fileStream << gene.fromId << "," << gene.toId << "," << gene.weight << std::endl;
	}
	fileStream.close();
}

void Chromosome::loadFromTextFile(std::string path)
{
	std::ifstream fileStream;
	fileStream.open(path);
	std::string delimiter = ",";
	int pos = 0;
	std::string output;
	std::string value;
	while(!fileStream.eof())
	{
		output = "";
		fileStream >> output;
		while ((pos = output.find(delimiter)) != std::string::npos) {
			value = output.substr(0, pos);
			std::cout << value << std::endl;
			output.erase(0, pos + delimiter.length());
		}
	}
	fileStream.close();
}
