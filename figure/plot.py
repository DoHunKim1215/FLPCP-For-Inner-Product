from decimal import Decimal, ROUND_UP

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.axes import Axes


class FLPCP:
    xs = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(1, 13)]

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.004800, 0.006400000, 0.015500000, 0.067200000, 0.413900000, 2.994500000, 23.708800000, 184.249200000, 1456.615700000, 11505.315300000, 91851.010700000, 736718.425400000]
        ys_precompute = [0.001000000, 0.001000000, 0.001800000, 0.004500000, 0.015000000, 0.049100000, 0.191800000, 0.739900000, 2.918000000, 11.587000000, 46.326800000, 187.940600000]
        coefficient = [0.000900000, 0.000900000, 0.001300000, 0.002900000, 0.005600000, 0.020400000, 0.065400000, 0.242100000, 0.880600000, 3.468800000, 13.759700000, 55.155900000]

        ax.plot(FLPCP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLPCP.xs, ys_precompute, color='green', marker='v', label='baseline (precomputed)')
        ax.plot(FLPCP.xs, coefficient, color='blue', marker='^', label='coefficient')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCP.xs, labels=FLPCP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.005900000, 0.009100000, 0.025100000, 0.089500000, 0.336300000, 1.310600000, 5.279600000, 21.010300000, 83.360600000, 329.118900000, 1315.491300000, 5319.381900000]
        coefficient = [0.000800000, 0.001500000, 0.002200000, 0.004000000, 0.007500000, 0.013800000, 0.027500000, 0.053200000, 0.105000000, 0.204600000, 0.401100000, 0.835900000]

        ax.plot(FLPCP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLPCP.xs, coefficient, color='blue', marker='^', label='coefficient')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCP.xs, labels=FLPCP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [56, 88, 152, 280, 536, 1048, 2072, 4120, 8216, 16408, 32792, 65560]

        ax.plot(FLPCP.xs, ys, color='red', marker='o')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.yscale('log')
        plt.yticks(ticks=[56, 100, 1000, 10000, 65560], labels=['56', r'$10^2$', r'$10^3$', r'$10^4$', '65560'])
        plt.xticks(ticks=FLPCP.xs, labels=FLPCP.x_labels)
        plt.tight_layout()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLPCP.prover_time()
        FLPCP.verifier_time()
        FLPCP.proof_size()


class FLIOP:
    xs = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048]
    x_labels = [fr'$2^{{{x}}}$' for x in range(1, 12)]

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [7.049200, 5.034900000, 6.798700000, 15.574800000, 50.306700000, 186.766200000, 725.408000000, 2880.788600000, 11558.043500000, 45870.369700000, 184166.701900000]
        ys_precompute = [1.698100000, 0.830400000, 0.769100000, 1.113100000, 1.698000000, 6.005200000, 6.269500000, 11.544100000, 22.718700000, 45.245800000, 91.684900000]
        ys_coefficient = [1.558300000, 0.662000000, 0.480700000, 0.498100000, 0.696800000, 1.064300000, 1.891800000, 3.578300000, 6.952000000, 13.742800000, 27.737400000]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_precompute, color='green', marker='v', label='baseline (precomputed)')
        ax.plot(FLIOP.xs, ys_coefficient, color='blue', marker='^', label='coefficient')

        plt.xlabel('Compression factor')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time_broken():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys_precompute = [1.698100000, 0.830400000, 0.769100000, 1.113100000, 1.698000000, 6.005200000, 6.269500000,
                         11.544100000, 22.718700000, 45.245800000, 91.684900000]
        ys_coefficient = [1.558300000, 0.662000000, 0.480700000, 0.498100000, 0.696800000, 1.064300000, 1.891800000,
                          3.578300000, 6.952000000, 13.742800000, 27.737400000]

        ax.plot(FLIOP.xs, ys_precompute, color='green', marker='v', label='baseline (precomputed)')
        ax.plot(FLIOP.xs, ys_coefficient, color='blue', marker='^', label='coefficient')

        plt.xlabel('Compression factor')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time_broken.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.008500000, 0.011700000, 0.029600000, 0.095300000, 0.027100000, 1.342300000, 0.461200000, 0.578300000, 1.948800000, 7.636200000, 30.850700000]
        ys_coefficient = [0.005300000, 0.003800000, 0.005300000, 0.006300000, 0.006300000, 0.015500000, 0.014400000, 0.020100000, 0.033700000, 0.067500000, 0.136300000]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_coefficient, color='blue', marker='^', label='coefficient')

        plt.xlabel('Compression factor')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def proof_length():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [320, 368, 512, 776, 1096, 2064, 2576, 4368, 8336, 16464, 32816]

        ax.plot(FLIOP.xs, ys, color='red', marker='o')

        plt.xlabel(r'Compression factor')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.yscale('log')
        plt.yticks(ticks=[320, 1000, 10000, 32816], labels=['320', r'$10^3$', r'$10^4$', '32816'])
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()


    @staticmethod
    def plot():
        FLIOP.prover_time()
        FLIOP.prover_time_broken()
        FLIOP.verifier_time()
        FLIOP.proof_length()


class FLIOPNet:
    xs = [2 ** i for i in range(5, 17)]
    x_labels = [rf'$2^{{{i}}}$' for i in range(5, 17)]

    @staticmethod
    def lan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [0.043880690, 0.049097384, 0.060402949, 0.080319643, 0.121041902, 0.198241902, 0.353958597, 0.663364162, 1.282280856, 2.532403115, 5.052508680, 10.526725374]
        baseline = [0.050275125, 0.061886254, 0.083197384, 0.123808514, 0.203519643, 0.359430773, 0.664641902, 1.278453032, 2.510864162, 5.036875291, 10.500386421, 22.624897550]

        ax.plot(FLIOPNet.xs, baseline, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOPNet.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPNet.xs, labels=FLIOPNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'lan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        optimum_total = np.array([0.043880690, 0.049097384, 0.060402949, 0.080319643, 0.121041902, 0.198241902, 0.353958597, 0.663364162, 1.282280856, 2.532403115, 5.052508680, 10.526725374])
        baseline_total = np.array([0.050275125, 0.061886254, 0.083197384, 0.123808514, 0.203519643, 0.359430773, 0.664641902, 1.278453032, 2.510864162, 5.036875291, 10.500386421, 22.624897550])

        baseline_prover = np.array([0.015000000, 0.026200000, 0.047000000, 0.087000000, 0.166200000, 0.321600000, 0.626100000, 1.239300000, 2.471100000, 4.996100000, 10.458600000, 22.581800000])
        baseline_verifier = np.array([0.005200000, 0.005600000, 0.006100000, 0.006700000, 0.007200000, 0.007700000, 0.008400000, 0.009000000, 0.009600000, 0.010600000, 0.011600000, 0.012900000])
        baseline_communication = np.array([0.030075125, 0.030086254, 0.030097384, 0.030108514, 0.030119643, 0.030130773, 0.030141902, 0.030153032, 0.030164162, 0.030175291, 0.030186421, 0.030197550])

        optimum_prover = np.array(
                [0.008900000, 0.013700000, 0.024800000, 0.044400000, 0.084400000, 0.161400000, 0.316600000, 0.625900000, 1.243700000, 2.493700000, 5.012400000, 10.485300000])
        optimum_verifier = np.array(
                [0.004900000, 0.005300000, 0.005500000, 0.005800000, 0.006500000, 0.006700000, 0.007200000, 0.007300000, 0.008400000, 0.008500000, 0.009900000, 0.011200000])
        optimum_communication = np.array(
                [0.030080690, 0.030097384, 0.030102949, 0.030119643, 0.030141902, 0.030141902, 0.030158597, 0.030164162, 0.030180856, 0.030203115, 0.030208680, 0.030225374])

        total_decrease = baseline_total - optimum_total

        optimum = {
            "prover time": (optimum_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (optimum_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (optimum_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(FLIOPNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_title(boolean + " increase / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=FLIOPNet.xs, labels=FLIOPNet.x_labels)
        fig.savefig(f'lan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [40.037000000, 40.044900000, 40.059900000, 40.084600000, 40.127500000, 40.208900000, 40.369000000, 40.680400000, 41.303800000, 42.556300000, 45.081000000, 50.561000000]
        baseline = [40.041800000, 40.056600000, 40.081100000, 40.124900000, 40.207800000, 40.366900000, 40.675300000, 41.292300000, 42.527900000, 45.057100000, 50.523800000, 62.651500000]

        ax.plot(FLIOPNet.xs, baseline, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOPNet.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPNet.xs, labels=FLIOPNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'wan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        optimum_total = np.array(
            [40.037000000, 40.044900000, 40.059900000, 40.084600000, 40.127500000, 40.208900000, 40.369000000, 40.680400000, 41.303800000, 42.556300000, 45.081000000, 50.561000000])
        baseline_total = np.array(
            [40.041800000, 40.056600000, 40.081100000, 40.124900000, 40.207800000, 40.366900000, 40.675300000, 41.292300000, 42.527900000, 45.057100000, 50.523800000, 62.651500000])

        baseline_prover = np.array(
            [0.015000000, 0.026200000, 0.047000000, 0.087000000, 0.166200000, 0.321600000, 0.626100000, 1.239300000, 2.471100000, 4.996100000, 10.458600000, 22.581800000])
        baseline_verifier = np.array(
            [0.005200000, 0.005600000, 0.006100000, 0.006700000, 0.007200000, 0.007700000, 0.008400000, 0.009000000, 0.009600000, 0.010600000, 0.011600000, 0.012900000])
        baseline_communication = np.array(
            [40.021600000, 40.024800000, 40.028000000, 40.031200000, 40.034400000, 40.037600000, 40.040800000, 40.044000000, 40.047200000, 40.050400000, 40.053600000, 40.056800000])

        optimum_prover = np.array(
            [0.008900000, 0.014700000, 0.024800000, 0.044400000, 0.085200000, 0.161400000, 0.319300000, 0.625900000, 1.245100000, 2.494000000, 5.012700000, 10.488300000])
        optimum_verifier = np.array(
            [0.004900000, 0.005400000, 0.005500000, 0.005800000, 0.006300000, 0.006700000, 0.007300000, 0.007300000, 0.008300000, 0.008700000, 0.009900000, 0.011100000])
        optimum_communication = np.array(
            [40.023200000, 40.024800000, 40.029600000, 40.034400000, 40.036000000, 40.040800000, 40.042400000, 40.047200000, 40.050400000, 40.053600000, 40.058400000, 40.061600000])

        total_decrease = baseline_total - optimum_total

        optimum = {
            "prover time": (optimum_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (optimum_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (optimum_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(FLIOPNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_title(boolean + " increase / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=FLIOPNet.xs, labels=FLIOPNet.x_labels)
        fig.savefig(f'wan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOPNet.lan()
        FLIOPNet.lan_portion()
        FLIOPNet.wan()
        FLIOPNet.wan_portion()


class FLIOP3PCNet:
    xs = [2 ** i for i in range(5, 17)]
    x_labels = [rf'$2^{{{i}}}$' for i in range(5, 17)]

    @staticmethod
    def lan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [0.053225208, 0.063236338, 0.081458597, 0.116769726, 0.183591986, 0.314208680, 0.573319810, 1.078947634, 2.099653198, 4.232875458, 8.434292152, 17.126997717]
        baseline = [0.065236338, 0.089758597, 0.123280856, 0.187603115, 0.312225374, 0.553247634, 1.033269893, 1.976792152, 3.901514411, 8.171436670, 16.253358930, 34.071481189]

        ax.plot(FLIOP3PCNet.xs, baseline, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOP3PCNet.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCNet.xs, labels=FLIOP3PCNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'lan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        optimum_total = np.array([0.053225208, 0.063236338, 0.081458597, 0.116769726, 0.183591986, 0.314208680, 0.573319810, 1.078947634, 2.099653198, 4.232875458, 8.434292152, 17.126997717])
        baseline_total = np.array([0.065236338, 0.089758597, 0.123280856, 0.187603115, 0.312225374, 0.553247634, 1.033269893, 1.976792152, 3.901514411, 8.171436670, 16.253358930, 34.071481189])

        baseline_prover = np.array([0.021100000, 0.039000000, 0.060800000, 0.102700000, 0.184600000, 0.342600000, 0.656500000, 1.268100000, 2.501800000, 5.286400000, 10.405900000, 22.206200000])
        baseline_verifier = np.array([0.014000000, 0.020600000, 0.032300000, 0.054700000, 0.097400000, 0.180400000, 0.346500000, 0.678400000, 1.369400000, 2.854700000, 5.817100000, 11.834900000])
        baseline_communication = np.array([0.030136338, 0.030158597, 0.030180856, 0.030203115, 0.030225374, 0.030247634, 0.030269893, 0.030292152, 0.030314411, 0.030336670, 0.030358930, 0.030381189])

        optimum_prover = np.array(
                [0.012900000, 0.018900000, 0.029100000, 0.051700000, 0.092300000, 0.170000000, 0.331600000, 0.635400000, 1.263400000, 2.542300000, 5.077200000, 10.514600000])
        optimum_verifier = np.array(
                [0.010200000, 0.014200000, 0.022200000, 0.034900000, 0.061100000, 0.114000000, 0.211500000, 0.413300000, 0.806000000, 1.660300000, 3.326800000, 6.582100000])
        optimum_communication = np.array(
                [0.030125208, 0.030136338, 0.030158597, 0.030169726, 0.030191986, 0.030208680, 0.030219810, 0.030247634, 0.030253198, 0.030275458, 0.030292152, 0.030297717])

        total_decrease = baseline_total - optimum_total

        optimum = {
            "prover time": (optimum_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (optimum_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (optimum_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(FLIOP3PCNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCNet.xs, labels=FLIOP3PCNet.x_labels)
        fig.savefig(f'lan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [40.057100000, 40.071000000, 40.096100000, 40.135300000, 40.207500000, 40.342800000, 40.604900000, 41.116800000, 42.140900000, 44.280500000, 48.486600000, 57.182300000]
        baseline = [40.074300000, 40.105200000, 40.145100000, 40.215800000, 40.346800000, 40.594200000, 41.080600000, 42.030500000, 43.961600000, 48.237900000, 56.326200000, 74.150700000]

        ax.plot(FLIOP3PCNet.xs, baseline, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOP3PCNet.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCNet.xs, labels=FLIOP3PCNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'wan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        optimum_total = np.array(
            [40.057100000, 40.071000000, 40.096100000, 40.135300000, 40.207500000, 40.342800000, 40.604900000, 41.116800000, 42.140900000, 44.280500000, 48.486600000, 57.182300000])
        baseline_total = np.array(
            [40.074300000, 40.105200000, 40.145100000, 40.215800000, 40.346800000, 40.594200000, 41.080600000, 42.030500000, 43.961600000, 48.237900000, 56.326200000, 74.150700000])

        baseline_prover = np.array(
            [0.021100000, 0.039000000, 0.060800000, 0.102700000, 0.184600000, 0.342600000, 0.656500000, 1.268100000, 2.501800000, 5.286400000, 10.405900000, 22.206200000])
        baseline_verifier = np.array(
            [0.014000000, 0.020600000, 0.032300000, 0.054700000, 0.097400000, 0.180400000, 0.346500000, 0.678400000, 1.369400000, 2.854700000, 5.817100000, 11.834900000])
        baseline_communication = np.array(
            [40.039200000, 40.045600000, 40.052000000, 40.058400000, 40.064800000, 40.071200000, 40.077600000, 40.084000000, 40.090400000, 40.096800000, 40.103200000, 40.109600000])

        optimum_prover = np.array(
            [0.014200000, 0.018100000, 0.029300000, 0.051300000, 0.093200000, 0.172300000, 0.331000000, 0.634500000, 1.262600000, 2.541500000, 5.080900000, 10.514600000])
        optimum_verifier = np.array(
            [0.010100000, 0.015300000, 0.022800000, 0.036800000, 0.062300000, 0.112100000, 0.212300000, 0.414300000, 0.807100000, 1.661400000, 3.324900000, 6.582100000])
        optimum_communication = np.array(
            [40.032800000, 40.037600000, 40.044000000, 40.047200000, 40.052000000, 40.058400000, 40.061600000, 40.068000000, 40.071200000, 40.077600000, 40.080800000, 40.085600000])

        total_decrease = baseline_total - optimum_total

        optimum = {
            "prover time": (optimum_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (optimum_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (optimum_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(FLIOP3PCNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCNet.xs, labels=FLIOP3PCNet.x_labels)
        fig.savefig(f'wan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOP3PCNet.lan()
        FLIOP3PCNet.lan_portion()
        FLIOP3PCNet.wan()
        FLIOP3PCNet.wan_portion()


class FLIOP3PCCoeffNet:
    xs = [2 ** i for i in range(5, 17)]
    x_labels = [rf'$2^{{{i}}}$' for i in range(5, 17)]

    @staticmethod
    def lan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [0.043014078, 0.047236338, 0.055975291, 0.072730939, 0.103497550, 0.165036504, 0.284275458, 0.518258763, 0.993886587, 1.923125541, 3.787831106, 8.169342235]
        baseline = [0.058136338, 0.073458597, 0.099580856, 0.148903115, 0.247425374, 0.437447634, 0.827169893, 1.687392152, 3.148814411, 6.240536670, 12.704258930, 26.442481189]

        ax.plot(FLIOP3PCCoeffNet.xs, baseline, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOP3PCCoeffNet.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCCoeffNet.xs, labels=FLIOP3PCCoeffNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'lan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        optimum_total = np.array([0.043014078, 0.047236338, 0.055975291, 0.072730939, 0.103497550, 0.165036504, 0.284275458, 0.518258763, 0.993886587, 1.923125541, 3.787831106, 8.169342235])
        baseline_total = np.array([0.058136338, 0.073458597, 0.099580856, 0.148903115, 0.247425374, 0.437447634, 0.827169893, 1.687392152, 3.148814411, 6.240536670, 12.704258930, 26.442481189])

        baseline_prover = np.array([0.019000000, 0.030100000, 0.050000000, 0.087400000, 0.164200000, 0.313200000, 0.618400000, 1.273600000, 2.419900000, 4.819300000, 9.913900000, 20.889200000])
        baseline_verifier = np.array([0.009000000, 0.013200000, 0.019400000, 0.031300000, 0.053000000, 0.094000000, 0.178500000, 0.383500000, 0.698600000, 1.390900000, 2.760000000, 5.522900000])
        baseline_communication = np.array([0.030136338, 0.030158597, 0.030180856, 0.030203115, 0.030225374, 0.030247634, 0.030269893, 0.030292152, 0.030314411, 0.030336670, 0.030358930, 0.030381189])

        optimum_prover = np.array(
                [0.008300000, 0.011300000, 0.017700000, 0.031300000, 0.055100000, 0.103400000, 0.198300000, 0.384300000, 0.766000000, 1.491400000, 2.974800000, 6.242400000])
        optimum_verifier = np.array(
                [0.004600000, 0.005800000, 0.008100000, 0.011200000, 0.018200000, 0.031400000, 0.055700000, 0.103700000, 0.197600000, 0.401400000, 0.782700000, 1.896600000])
        optimum_communication = np.array(
                [0.030114078, 0.030136338, 0.030175291, 0.030230939, 0.030197550, 0.030236504, 0.030275458, 0.030258763, 0.030286587, 0.030325541, 0.030331106, 0.030342235])

        total_decrease = baseline_total - optimum_total

        optimum = {
            "prover time": (optimum_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (optimum_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (optimum_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(FLIOP3PCCoeffNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_title(boolean + " variance / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, 0., max], labels=[str(min), '0', str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(0., color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCCoeffNet.xs, labels=FLIOP3PCCoeffNet.x_labels)
        fig.savefig(f'lan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [40.045700000, 40.056300000, 40.071600000, 40.092500000, 40.130100000, 40.198200000, 40.322300000, 40.561600000, 41.046000000, 41.980800000, 43.848800000, 48.237400000]
        baseline = [40.067200000, 40.088900000, 40.121400000, 40.177100000, 40.282000000, 40.478400000, 40.874500000, 41.741100000, 43.208900000, 46.307000000, 52.777100000, 66.521700000]

        ax.plot(FLIOP3PCCoeffNet.xs, baseline, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOP3PCCoeffNet.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCCoeffNet.xs, labels=FLIOP3PCCoeffNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'wan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        optimum_total = np.array(
            [40.045700000, 40.056300000, 40.071600000, 40.092500000, 40.130100000, 40.198200000, 40.322300000, 40.561600000, 41.046000000, 41.980800000, 43.848800000, 48.237400000])
        baseline_total = np.array(
            [40.067200000, 40.088900000, 40.121400000, 40.177100000, 40.282000000, 40.478400000, 40.874500000, 41.741100000, 43.208900000, 46.307000000, 52.777100000, 66.521700000])

        baseline_prover = np.array(
            [0.019000000, 0.030100000, 0.050000000, 0.087400000, 0.164200000, 0.313200000, 0.618400000, 1.273600000, 2.419900000, 4.819300000, 9.913900000, 20.889200000])
        baseline_verifier = np.array(
            [0.009000000, 0.013200000, 0.019400000, 0.031300000, 0.053000000, 0.094000000, 0.178500000, 0.383500000, 0.698600000, 1.390900000, 2.760000000, 5.522900000])
        baseline_communication = np.array(
            [40.039200000, 40.045600000, 40.052000000, 40.058400000, 40.064800000, 40.071200000, 40.077600000, 40.084000000, 40.090400000, 40.096800000, 40.103200000, 40.109600000])

        optimum_prover = np.array(
            [0.008300000, 0.011300000, 0.020100000, 0.031900000, 0.055100000, 0.105100000, 0.198600000, 0.384400000, 0.766000000, 1.494400000, 2.974900000, 6.242400000])
        optimum_verifier = np.array(
            [0.004600000, 0.005800000, 0.009100000, 0.011800000, 0.018200000, 0.033100000, 0.058900000, 0.104400000, 0.197600000, 0.402400000, 0.783500000, 1.896600000])
        optimum_communication = np.array(
            [40.032800000, 40.039200000, 40.042400000, 40.048800000, 40.056800000, 40.060000000, 40.064800000, 40.072800000, 40.082400000, 40.084000000, 40.090400000, 40.098400000])

        total_decrease = baseline_total - optimum_total

        optimum = {
            "prover time": (optimum_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (optimum_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (optimum_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(FLIOP3PCCoeffNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=FLIOP3PCCoeffNet.xs, labels=FLIOP3PCCoeffNet.x_labels)
        fig.savefig(f'wan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOP3PCCoeffNet.lan()
        FLIOP3PCCoeffNet.lan_portion()
        FLIOP3PCCoeffNet.wan()
        FLIOP3PCCoeffNet.wan_portion()


class ComparisonFLIOP3PCNet:
    xs = [2 ** i for i in range(5, 17)]
    x_labels = [rf'$2^{{{i}}}$' for i in range(5, 17)]

    @staticmethod
    def lan():
        fig, ax = plt.subplots(figsize=(5, 4))

        coeff = [0.043014078, 0.047236338, 0.055975291, 0.072730939, 0.103497550, 0.165036504, 0.284275458, 0.518258763, 0.993886587, 1.923125541, 3.787831106, 8.169342235]
        baseline = [0.053225208, 0.063236338, 0.081458597, 0.116769726, 0.183591986, 0.314208680, 0.573319810, 1.078947634, 2.099653198, 4.232875458, 8.434292152, 17.126997717]

        ax.plot(ComparisonFLIOP3PCNet.xs, baseline, color='red', marker='o', label='baseline')
        ax.plot(ComparisonFLIOP3PCNet.xs, coeff, color='blue', marker='^', label='coefficient')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=ComparisonFLIOP3PCNet.xs, labels=ComparisonFLIOP3PCNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'lan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        coeff_total = np.array([0.043014078, 0.047236338, 0.055975291, 0.072730939, 0.103497550, 0.165036504, 0.284275458, 0.518258763, 0.993886587, 1.923125541, 3.787831106, 8.169342235])
        baseline_total = np.array([0.053225208, 0.063236338, 0.081458597, 0.116769726, 0.183591986, 0.314208680, 0.573319810, 1.078947634, 2.099653198, 4.232875458, 8.434292152, 17.126997717])

        baseline_prover = np.array(
            [0.012900000, 0.018900000, 0.029100000, 0.051700000, 0.092300000, 0.170000000, 0.331600000, 0.635400000,
             1.263400000, 2.542300000, 5.077200000, 10.514600000])
        baseline_verifier = np.array(
            [0.010200000, 0.014200000, 0.022200000, 0.034900000, 0.061100000, 0.114000000, 0.211500000, 0.413300000,
             0.806000000, 1.660300000, 3.326800000, 6.582100000])
        baseline_communication = np.array(
            [0.030125208, 0.030136338, 0.030158597, 0.030169726, 0.030191986, 0.030208680, 0.030219810, 0.030247634,
             0.030253198, 0.030275458, 0.030292152, 0.030297717])

        coeff_prover = np.array(
                [0.008300000, 0.011300000, 0.017700000, 0.031300000, 0.055100000, 0.103400000, 0.198300000, 0.384300000, 0.766000000, 1.491400000, 2.974800000, 6.242400000])
        coeff_verifier = np.array(
                [0.004600000, 0.005800000, 0.008100000, 0.011200000, 0.018200000, 0.031400000, 0.055700000, 0.103700000, 0.197600000, 0.401400000, 0.782700000, 1.896600000])
        coeff_communication = np.array(
                [0.030114078, 0.030136338, 0.030175291, 0.030230939, 0.030197550, 0.030236504, 0.030275458, 0.030258763, 0.030286587, 0.030325541, 0.030331106, 0.030342235])

        total_decrease = baseline_total - coeff_total

        optimum = {
            "prover time": (coeff_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (coeff_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (coeff_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(ComparisonFLIOP3PCNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_title(boolean + " variance / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, 0., max], labels=[str(min), '0', str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(0., color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=ComparisonFLIOP3PCNet.xs, labels=ComparisonFLIOP3PCNet.x_labels)
        fig.savefig(f'lan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan():
        fig, ax = plt.subplots(figsize=(5, 4))

        coeff = [40.045700000, 40.056300000, 40.071600000, 40.092500000, 40.130100000, 40.198200000, 40.322300000, 40.561600000, 41.046000000, 41.980800000, 43.848800000, 48.237400000]
        baseline = [40.057100000, 40.071000000, 40.096100000, 40.135300000, 40.207500000, 40.342800000, 40.604900000, 41.116800000, 42.140900000, 44.280500000, 48.486600000, 57.182300000]

        ax.plot(ComparisonFLIOP3PCNet.xs, baseline, color='red', marker='o', label='baseline')
        ax.plot(ComparisonFLIOP3PCNet.xs, coeff, color='blue', marker='^', label='coefficient')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=ComparisonFLIOP3PCNet.xs, labels=ComparisonFLIOP3PCNet.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'wan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_portion():
        fig, axs = plt.subplots(nrows=3, ncols=1, figsize=(5, 4), sharex=True)

        coeff_total = np.array(
            [40.045700000, 40.056300000, 40.071600000, 40.092500000, 40.130100000, 40.198200000, 40.322300000, 40.561600000, 41.046000000, 41.980800000, 43.848800000, 48.237400000])
        baseline_total = np.array(
            [40.057100000, 40.071000000, 40.096100000, 40.135300000, 40.207500000, 40.342800000, 40.604900000, 41.116800000, 42.140900000, 44.280500000, 48.486600000, 57.182300000])

        baseline_prover = np.array(
            [0.014200000, 0.018100000, 0.029300000, 0.051300000, 0.093200000, 0.172300000, 0.331000000, 0.634500000,
             1.262600000, 2.541500000, 5.080900000, 10.514600000])
        baseline_verifier = np.array(
            [0.010100000, 0.015300000, 0.022800000, 0.036800000, 0.062300000, 0.112100000, 0.212300000, 0.414300000,
             0.807100000, 1.661400000, 3.324900000, 6.582100000])
        baseline_communication = np.array(
            [40.032800000, 40.037600000, 40.044000000, 40.047200000, 40.052000000, 40.058400000, 40.061600000,
             40.068000000, 40.071200000, 40.077600000, 40.080800000, 40.085600000])

        coeff_prover = np.array(
            [0.008300000, 0.011300000, 0.020100000, 0.031900000, 0.055100000, 0.105100000, 0.198600000, 0.384400000, 0.766000000, 1.494400000, 2.974900000, 6.242400000])
        coeff_verifier = np.array(
            [0.004600000, 0.005800000, 0.009100000, 0.011800000, 0.018200000, 0.033100000, 0.058900000, 0.104400000, 0.197600000, 0.402400000, 0.783500000, 1.896600000])
        coeff_communication = np.array(
            [40.032800000, 40.039200000, 40.042400000, 40.048800000, 40.056800000, 40.060000000, 40.064800000, 40.072800000, 40.082400000, 40.084000000, 40.090400000, 40.098400000])

        total_decrease = baseline_total - coeff_total

        optimum = {
            "prover time": (coeff_prover - baseline_prover) / total_decrease * 100.,
            "verifier time": (coeff_verifier - baseline_verifier) / total_decrease * 100.,
            "communication time": (coeff_communication - baseline_communication) / total_decrease * 100.,
        }

        color = ['tab:green', 'tab:cyan', 'tab:orange']

        for idx, (boolean, weight_count) in enumerate(optimum.items()):
            ax: Axes = axs[idx]
            ax.plot(ComparisonFLIOP3PCNet.xs, weight_count, color=color[idx], marker='^')
            ax.set(xlabel='Input vector length')
            min = float(Decimal(np.min(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            max = float(Decimal(np.max(optimum[boolean]).item()).quantize(Decimal(".001"), rounding=ROUND_UP))
            if boolean == 'prover time' or boolean == 'verifier time':
                ax.set_title(boolean + " decrease / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, max], labels=[str(min), str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
            else:
                ax.set_title(boolean + " variance / total decrease (%)", fontdict={'fontsize': 10})
                ax.set_ylim([min - (max - min) * 0.1, max + (max - min) * 0.1])
                ax.set_yticks(ticks=[min, 0, max], labels=[str(min), '0', str(max)])
                ax.axhline(min, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(max, color='black', linestyle='-', linewidth=0.5)
                ax.axhline(0., color='black', linestyle='-', linewidth=0.5)

        for ax in axs:
            ax.label_outer()

        fig.tight_layout()
        plt.xscale('log')
        plt.xticks(ticks=ComparisonFLIOP3PCNet.xs, labels=ComparisonFLIOP3PCNet.x_labels)
        fig.savefig(f'wan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        ComparisonFLIOP3PCNet.lan()
        ComparisonFLIOP3PCNet.lan_portion()
        ComparisonFLIOP3PCNet.wan()
        ComparisonFLIOP3PCNet.wan_portion()

if __name__ == '__main__':
    ComparisonFLIOP3PCNet.plot()
