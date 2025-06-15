#include <App.h>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>

class VNIManager {
private:
    std::set<int> allocatedVNIs;
    int nextVNI;

public:
    VNIManager(int startVNI = 4096, int maxVNI = 16777214) : nextVNI(startVNI) {}

    int create() {
        while (allocatedVNIs.find(nextVNI) != allocatedVNIs.end()) {
            nextVNI++; // Find the next available VNI
            if (nextVNI > 16777214) {
                std::cerr << "Error: No available VNI left!\n";
                return -1;
            }
        }
        allocatedVNIs.insert(nextVNI);
        return nextVNI++;
    }

    void deleteVNI(int vni) {
        if (allocatedVNIs.erase(vni)) {
            std::cout << "VNI " << vni << " has been released.\n";
            if (vni < nextVNI) {
                nextVNI = vni; // Reuse lowest possible VNI
            }
        } else {
            std::cerr << "Error: VNI " << vni << " was not allocated!\n";
        }
    }
};

int main() {
    VNIManager vniManager;
    std::unordered_map<int, std::vector<std::string>> vxlans;

    uWS::App().get("/VXLAN/create", [&vniManager](auto *res, auto *req) {
        int vni = vniManager.create();
        if (vni != -1) {
            res->end("Allocated VNI: " + std::to_string(vni));
        } else {
            res->end("Error: No available VNIs.");
        }
    }).get("/VXLAN/associate/:vni/:device/:ip", [&vxlans](auto *res, auto *req) {
        std::string vniStr = std::string(req->getParameter(0)); // Convert string_view to string
        std::string device = std::string(req->getParameter(1)); // Convert string_view to string
        std::string localIP = std::string(req->getParameter(2)); // Convert string_view to string
        int vni;

        try {
            vni = std::stoi(vniStr); // Convert to integer
        } catch (const std::exception &e) {
            res->end("Error: Invalid VNI format.");
            return;
        }

        // Placeholder: Verify if VNI and device exist
        bool vniExists = true; // Replace with actual verification logic
        bool deviceExists = true; // Replace with actual verification logic

        if (vniExists && deviceExists) {
            std::cout << "ip link add br" << vni << "type bridge" << std::endl;
	    std::cout << "ip link set br" << vni << " up" << std::endl;
	    std::cout << "ip link add vxlan" << vni << " type vxlan id " << vni << "dev " << device << " dstport 4789 local " << localIP << " nolearning" << std::endl;
	    std::cout << "ip link set vxlan" << vni << " up"  << std::endl;
	    std::cout << "ip link set vxlan" << vni << " master br" << vni << std::endl;

	    // do this for each other interface associated with VNI
	    std::cout << "fdb append <remote unique generated MAC> dev vxlan" << vni << " dst <remoteIP>" << std::endl;
	    std::cout << "ip link add vni" << vni << " type dummy" << std::endl;
	    std::cout << "ip link set vni" << vni << " address <unique  mac>" << std::endl;
	    std::cout << "ip link set vni" << vni << " up" << std::endl;
	    std::cout << "ip link set vni" << vni << " master br" << vni << std::endl;
	    std::cout << "ip addr add 192.168.1.1/24 dev vni" << vni << std::endl;

	    // add this device to the vxlans so we track needed info to tear down
	    // and also to fdb append macs to enable ARP
	    vxlans[vni].emplace_back(localIP);
            res->end("Success: added endpoint " + localIP + " to VNI " + std::to_string(vni));
        } else {
            res->end("Error: VNI or Device does not exist.");
        }
    }).get("/VXLAN/:vni", [&vxlans](auto *res, auto *req) {
        int vni = std::stoi(std::string(req->getParameter(0)));
	std::string ipString = std::accumulate(vxlans[vni].begin(), vxlans[vni].end(), std::string(),
					       [](const std::string& a, const std::string& b) {
						   return a.empty() ? b : a + " " + b;
					       });

        res->end("VNI: " + std::to_string(vni) + " contains the following endpoints: " + ipString);
    }).listen(9001, [](auto *token) {
        if (token) {
            std::cout << "Server listening on port 9001\n";
        }
    }).run();

    return 0;
}


