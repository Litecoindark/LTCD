// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 50, uint256("0x3145c1a9874355a6dd6974788581d12454542de407c1ed7a6e749faa24ae9f80"))
        ( 100, uint256("0x59cc0628179755b56e89eb279962749a4c29b03b15c4707ef13cd929d1184e17"))
        ( 150, uint256("0x177a415b863f283337fca2eabeb37d9050ed6a28f026edd3089f4bd1cf6983e4"))
        ( 200, uint256("0x62b166411cd77b7ec40700ad3bf0655f2ee9c9070cde51d3cdfefd5a0991633a"))
        ( 400, uint256("0xe409b3dd7afc41681fb95dd3b0ecb1965756612765be0d5b8b20fadc876e44e2"))
        ( 500, uint256("0x177c4ac380ef413f297e69e6d98b11c546ac21299e7fb96f5e729dc74e6d2840"))
        ( 600, uint256("0x871b47e02d586ac2f8458b4b8bf3b821752ad79ba82be8af42b397b72588d7dc"))
        ( 700, uint256("0xef95e03d8d34efde9dcff9c69dc06139cd5ebc799672d37e09f1cdac2dfa696d"))
        ( 800, uint256("0xdb8228716e3ee18b15dbc63812043504ca5a53a7efdbe85a71917a85b3446f42"))
        ( 1000, uint256("0x8b43e13cdb15c856161194bd39e3c61f65a1b8b1ea6bb417be64777b5c8f184e"))
        ( 1500, uint256("0xfcba0ff54c6b784cd0571ce4deb8bce5239cadc10cf3585c3b5cca6548bac5e7"))
        ( 2500, uint256("0x18c7ea7311df97744082ccfee0bb6ec6198ee5063f6262777eb379817f4dd420"))
        ( 5000, uint256("0x97f43986ccd5b8fcb655187a3c5f5dee1fc6c06d30f19489c0a32fb79b705dd3"))                        
        ( 7500, uint256("0x6dcdc428bacafef41b5a7d9d465f83dd4d0cbe582beecc04f54fb010999c5760"))
        ( 9000, uint256("0x9d73d2ddffb61293561100150ad36002697cdbff0cd6514698933ac0f2271359"))
        ( 10000, uint256("0xdbc365e31501d29778a92978ae4ea6618434a42ea7d17e4f196a1e02f96fdbe6"))        
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1383887110, // * UNIX timestamp of last checkpoint block
        2179203,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        8000.0     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet = 
        boost::assign::map_list_of
        (   546, uint256("000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70"))
        ( 35000, uint256("2af959ab4f12111ce947479bfcef16702485f04afd95210aa90fde7d1e4a64ad"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1369685559,
        37581,
        300
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (fTestNet) return true; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (fTestNet) return 0; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (fTestNet) return NULL; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
