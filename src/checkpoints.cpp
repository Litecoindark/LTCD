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
        ( 0, uint256("0x0e286802e2a399cad2f4a6e41f0584a162dddfb303b78839b690e77dee2310e1"))
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
        ( 11000, uint256("0xa44804e286cdadf919b370f1701c59c9da6f05831b8f386968cbcb2efd9dbe53"))
        ( 12000, uint256("0x190ca505a4d75a22bfdac780583c35f0aac4fa6412f4af07df01ccf66defa391"))
        ( 13000, uint256("0x4c64525e39e243d7e0348fca39a893f1a0ca51762ad10384ac9d32cdc1d28af8"))
        ( 14000, uint256("0x0f1dbf47e04a3273951d907d2edf583690e6839f1bd1dc2da2d0674815ff98d9"))
        ( 15000, uint256("0x369a6d6c51920ace864469898918da9fc6b22c58ef416c8f2a573f7e3b5c9b37"))
        ( 16000, uint256("0x2eaadf4e77d6ed0a5d4317ba5f053ef57465c02ef027fe893fe4ade19e97012b"))
        ( 17000, uint256("0xce696c0f3e4f295df99ee5a90fbe2497639efccacb3e30002d1b1044a61b3a31"))
        ( 18000, uint256("0x567a2c5ea475e50e52278a1ddb7d637af02a74b78f61b045b2deecbbbd5357c3"))
        ( 19000, uint256("0x0cce810d4119bd0360e9d5f9570818aa8ce2cea9a167750682f55b574c45dc87"))
        ( 20000, uint256("0xc4203f4bd41907ed62f4153eed6473a995a02fccdd198ee188eff60080bfca4e"))
        ( 21000, uint256("0x1ced6220adf44090c035f867f2ca19a810f9bbcadfaa2d49c17ba17a7eaeacbf"))
        ( 22000, uint256("0x140307ebf4eb9017c79b40412204ef2e848f5bb937719c885002c9ca08e5c4de"))
		(25000, uint256("0xce10c6962536a97978b6221be16c4bbed69c3b86ca980ed93627a757b822e296"))
		(27500, uint256("0x2c60ca5b8b781970a4a9d4179c5c3027356fc50e2c367f0c530f93b43ddcdb94"))
		(30000, uint256("0xa3d75c5334f2030cd550cf11272a6077b2b3407e45224599028be42700dd90e7"))
		(32000, uint256("0xb819c728e5ee2d37cd19c8cf65c48e601fe273e2d816d22dce42d81f3ef09161"))


        ;


    static const CCheckpointData data = {
            &mapCheckpoints,
			1411883734, // * UNIX timestamp of last checkpoint block
            2179203,    // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            8000.0     // * estimated number of transactions per day after checkpoint
        };

    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        (   546, uint256("0xa0fea99a6897f531600c8ae53367b126824fd6a847b2b2b73817a95b8e27e602"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1365458829,
        547,
        576
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
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
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
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
