import numpy as np
from matplotlib import pyplot as plt, transforms


class FLPCP:
    xs = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(1, 13)]

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.003267, 0.0054, 0.0155, 0.0683, 0.4138, 3.0912, 23.3190, 185.4024, 1440.2577, 11518.6699, 91764.1160, 732102.8061]
        ys_precompute = [0.0007, 0.0008, 0.0017, 0.0048, 0.0133, 0.0481, 0.1908, 0.7279, 2.8545, 11.4161, 45.4542, 182.6980]
        coefficient = [0.0004, 0.0004, 0.0008, 0.0016, 0.0045, 0.0151, 0.0652, 0.2218, 0.8604, 3.4198, 13.6018, 54.7555]

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

        ys = [0.0033, 0.0079, 0.0258, 0.0906, 0.3471, 1.3382, 5.2696, 21.3635, 83.5475, 334.4002, 1333.5014, 5372.8210]
        coefficient = [0.0009, 0.0014, 0.0022, 0.0033, 0.0069, 0.0135, 0.0280, 0.0661, 0.1009, 0.2030, 0.3892, 0.7836]

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
        fig, ax = plt.subplots(figsize=(4, 3))

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

        ys = [7.032490, 5.0206, 6.7418, 15.5288, 50.4706, 186.3727, 731.9195, 2887.4468, 11535.4456, 45877.5325, 183184.5936]
        ys_precompute = [1.6459, 0.8230, 0.7588, 1.1029, 1.6629, 5.9558, 6.2380, 11.3660, 22.5444, 44.8601, 89.7458]
        ys_coefficient = [1.5261, 0.6485, 0.4768, 0.4819, 0.6399, 1.0453, 1.8920, 3.5483, 6.9634, 13.8757, 27.4585]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_precompute, color='green', marker='v', label='baseline (precomputed)')
        ax.plot(FLIOP.xs, ys_coefficient, color='blue', marker='^', label='coefficient')

        plt.xlabel('Compression factor')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [6.2625, 4.6627, 6.4649, 15.2577, 49.9712, 187.0101, 731.3495, 2880.3821, 11508.7314, 46004.6838, 183175.5013]
        ys_precomputation = [0.8911, 0.4615, 0.4874, 0.7381, 1.1306, 3.4052, 4.4227, 8.3598, 17.4641, 38.5578, 91.2784]
        ys_coefficient = [0.4995, 0.2237, 0.1515, 0.1054, 0.0893, 0.0914, 0.0841, 0.1098, 0.1035, 0.1279, 0.1827]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_precomputation, color='green', marker='v', label='baseline (precomputed)')
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
        fig, ax = plt.subplots(figsize=(4, 3))

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
    def lan_time():
        fig, ax = plt.subplots(figsize=(4, 3))

        ys = [0.0302, 0.0302, 0.0303, 0.0304, 0.0304, 0.0311, 0.0311, 0.0316, 0.0330, 0.0358, 0.0415]

        ax.plot(FLIOP.xs, ys, color='red', marker='o')

        plt.xlabel(r'Compression factor')
        plt.ylabel('Communication time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        fig.savefig(f'lan_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_time():
        fig, ax = plt.subplots(figsize=(4, 3))

        ys = [40.0448, 40.0528, 40.0736, 40.1128, 40.1256, 40.3184, 40.3184, 40.4720, 40.8560, 41.6624, 43.2944]

        ax.plot(FLIOP.xs, ys, color='red', marker='o')

        plt.xlabel(r'Compression factor')
        plt.ylabel('Communication time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        fig.savefig(f'wan_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOP.prover_time()
        FLIOP.verifier_time()
        FLIOP.proof_length()
        FLIOP.lan_time()
        FLIOP.wan_time()


class FLIOP_MPC:
    xs = [2 ** i for i in range(5, 17)]
    x_labels = [rf'$2^{{{i}}}$' for i in range(5, 17)]

    @staticmethod
    def lan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [0.0814, 0.0908, 0.1098, 0.1434, 0.2091, 0.3409, 0.5946, 1.1032, 2.1291, 4.1517, 8.4775, 16.9121]
        constant = [0.0923, 0.1112, 0.1445, 0.2137, 0.3368, 0.5767, 1.0501, 2.0549, 3.9612, 7.8773, 16.1953, 35.3174]

        ax.plot(FLIOP_MPC.xs, constant, color='red', marker='o', label='constant compression ($\lambda = 2$)')
        ax.plot(FLIOP_MPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP_MPC.xs, labels=FLIOP_MPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_lan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_payload():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [336, 368, 448, 456, 520, 544, 592, 640, 680, 760, 824, 832]
        constant = [352, 408, 464, 520, 576, 632, 688, 744, 800, 856, 912, 968]

        ax.plot(FLIOP_MPC.xs, constant, color='red', marker='o', label='constant compression ($\lambda = 2$)')
        ax.plot(FLIOP_MPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total payload size in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP_MPC.xs, labels=FLIOP_MPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_lan_payload.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_portion():
        fig, ax = plt.subplots(figsize=(5, 4))

        bottom = np.zeros(6)
        width = 0.7

        labels = ["constant", "optimum"]
        groups = ["3pc", "6pc", "9pc"]
        color = ['tab:cyan', 'tab:orange']

        times = {
            "prover time": np.array([23.0859, 10.2084, 23.7406, 10.5304, 23.8152, 10.4393]),
            "verifier time": np.array([12.1712, 6.6434, 12.4547, 6.5573, 12.3998, 6.6928]),
        }

        col_pos = np.array([0, 1, 2.5, 3.5, 5, 6])
        for idx, (label, element) in enumerate(times.items()):
            rects = ax.bar(col_pos, element, width, label=label, color=color[idx], bottom=bottom, edgecolor='black')
            ax.bar_label(rects, label_type="center", fmt=lambda x: '%.2f' % x, fontsize='small')
            bottom += element

        plt.xticks(ticks=col_pos, labels=np.tile(labels, len(groups)), rotation=20)

        plt.xlabel('')
        col_pos = col_pos.reshape(-1, len(labels)).mean(axis=1)
        trans = transforms.blended_transform_factory(ax.transData, ax.transAxes)

        ax.text(6.65, 44.5, r'(communication time $\approx$ 0.06 ms)', fontsize=10, ha='right', va='top')
        for pos, label in zip(col_pos, groups):
            ax.text(pos, -0.22, label, transform=trans, ha='center', va='bottom', fontsize=12)
        for pos in (col_pos[:-1] + col_pos[1:]) / 2:
            ax.axvline(pos, 0, -0.22, ls=':', color='black', clip_on=False)

        ax.set_ylim(0, 45)
        plt.ylabel(r'Total time of input length $2^{16}$ in LAN (ms)')
        plt.tight_layout()

        handles, labels = plt.gca().get_legend_handles_labels()
        order = [1, 0]
        plt.legend(loc='upper left', handles=[handles[idx] for idx in order], labels=[labels[idx] for idx in order])
        plt.savefig(f'lan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [80.0518, 80.0671, 80.0895, 80.1281, 80.1985, 80.3351, 80.5933, 81.1069, 82.1366, 84.1646, 88.4968, 96.9343]
        constant = [80.0674, 80.0919, 80.1307, 80.2055, 80.3342, 80.5797, 81.0587, 82.0690, 83.9809, 87.9026, 96.2262, 115.3539]

        ax.plot(FLIOP_MPC.xs, constant, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOP_MPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP_MPC.xs, labels=FLIOP_MPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_wan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_payload():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [304, 352, 392, 440, 488, 544, 576, 632, 664, 720, 792, 816]
        constant = [352, 408, 464, 520, 576, 632, 688, 744, 800, 856, 912, 968]

        ax.plot(FLIOP_MPC.xs, constant, color='red', marker='o', label='constant compression ($\lambda = 2$)')
        ax.plot(FLIOP_MPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total payload size in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP_MPC.xs, labels=FLIOP_MPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_wan_payload.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_portion():
        fig, ax = plt.subplots(figsize=(5, 4))

        bottom = np.zeros(6)
        width = 0.7

        labels = ["constant", "optimum"]
        groups = ["3pc", "6pc", "9pc"]
        color = ['tab:green', 'tab:cyan', 'tab:orange']

        times = {
            "communication time": np.array(
                [80.0968, 80.0816, 80.0968, 80.0800, 80.0968, 80.0816]),
            "prover time": np.array(
                [23.0859, 10.2080, 23.7406, 10.5332, 23.8152, 10.4382]),
            "verifier time": np.array(
                [12.1712, 6.6447, 12.4547, 6.5572, 12.3998, 6.6949]),
        }

        col_pos = np.array([0, 1, 2.5, 3.5, 5, 6])
        for idx, (label, element) in enumerate(times.items()):
            rects = ax.bar(col_pos, element, width, label=label, color=color[idx], bottom=bottom, edgecolor='black')
            ax.bar_label(rects, label_type="center", fmt=lambda x: '%.2f' % x, fontsize='small')
            bottom += element

        plt.xticks(ticks=col_pos, labels=np.tile(labels, len(groups)), rotation=20)

        plt.xlabel('')
        col_pos = col_pos.reshape(-1, len(labels)).mean(axis=1)
        trans = transforms.blended_transform_factory(ax.transData, ax.transAxes)

        for pos, label in zip(col_pos, groups):
            ax.text(pos, -0.22, label, transform=trans, ha='center', va='bottom', fontsize=12)
        for pos in (col_pos[:-1] + col_pos[1:]) / 2:
            ax.axvline(pos, 0, -0.22, ls=':', color='black', clip_on=False)

        ax.set_ylim(0, 160)
        plt.ylabel(r'Total time of input length $2^{16}$ in WAN (ms)')
        plt.tight_layout()

        handles, labels = plt.gca().get_legend_handles_labels()
        order = [2, 1, 0]
        plt.legend(loc='upper left', handles=[handles[idx] for idx in order], labels=[labels[idx] for idx in order])
        plt.savefig(f'wan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOP_MPC.lan()
        FLIOP_MPC.lan_portion()
        FLIOP_MPC.lan_payload()
        FLIOP_MPC.wan()
        FLIOP_MPC.wan_portion()
        FLIOP_MPC.wan_payload()


class FLIOPCoefficientMPC:
    xs = [2 ** i for i in range(5, 17)]
    x_labels = [rf'$2^{{{i}}}$' for i in range(5, 17)]

    @staticmethod
    def lan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [0.0731, 0.0768, 0.0842, 0.1020, 0.1314, 0.1923, 0.3132, 0.5426, 1.0127, 1.9534, 3.8155, 7.8995]
        constant = [0.0901, 0.1048, 0.1306, 0.1789, 0.2714, 0.4505, 0.7990, 1.4911, 2.8836, 6.0377, 12.0365, 25.3062]

        ax.plot(FLIOPCoefficientMPC.xs, constant, color='red', marker='o', label='constant compression ($\lambda = 2$)')
        ax.plot(FLIOPCoefficientMPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoefficientMPC.xs, labels=FLIOPCoefficientMPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_lan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_portion():
        fig, ax = plt.subplots(figsize=(5, 4))

        bottom = np.zeros(6)
        width = 0.7

        labels = ["constant", "optimum"]
        groups = ["3pc", "6pc", "9pc"]
        color = ['tab:cyan', 'tab:orange']

        times = {
            "prover time": np.array(
                [19.9454, 6.2265, 19.5302, 6.1122, 19.8861, 6.2482]),
            "verifier time": np.array(
                [5.3005, 1.6127, 5.2846, 1.6245, 5.2921, 1.5158]),
        }

        col_pos = np.array([0, 1, 2.5, 3.5, 5, 6])
        for idx, (label, element) in enumerate(times.items()):
            rects = ax.bar(col_pos, element, width, label=label, color=color[idx], bottom=bottom, edgecolor='black')
            ax.bar_label(rects, label_type="center", fmt=lambda x: '%.2f' % x, fontsize='small')
            bottom += element

        plt.xticks(ticks=col_pos, labels=np.tile(labels, len(groups)), rotation=20)

        plt.xlabel('')
        col_pos = col_pos.reshape(-1, len(labels)).mean(axis=1)
        trans = transforms.blended_transform_factory(ax.transData, ax.transAxes)

        ax.text(6.65, 31.5, r'(communication time $\approx$ 0.06 ms)', fontsize=10, ha='right', va='top')
        for pos, label in zip(col_pos, groups):
            ax.text(pos, -0.22, label, transform=trans, ha='center', va='bottom', fontsize=12)
        for pos in (col_pos[:-1] + col_pos[1:]) / 2:
            ax.axvline(pos, 0, -0.22, ls=':', color='black', clip_on=False)

        ax.set_ylim(0, 32)
        plt.ylabel(r'Total time of input length $2^{16}$ in LAN (ms)')
        plt.tight_layout()

        handles, labels = plt.gca().get_legend_handles_labels()
        order = [1, 0]
        plt.legend(loc='upper left', handles=[handles[idx] for idx in order], labels=[labels[idx] for idx in order])
        plt.savefig(f'lan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_payload():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [344, 392, 488, 648, 560, 640, 800, 712, 792, 904, 1096, 960]
        constant = [352, 408, 464, 520, 576, 632, 688, 744, 800, 856, 912, 968]

        ax.plot(FLIOP_MPC.xs, constant, color='red', marker='o', label='constant compression ($\lambda = 2$)')
        ax.plot(FLIOP_MPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total payload size in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP_MPC.xs, labels=FLIOP_MPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_lan_payload.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [80.0444, 80.0551, 80.0682, 80.0898, 80.1261, 80.1922, 80.3171, 80.5528, 81.0290, 81.9753, 83.8430, 87.9352]
        baseline = [80.0652, 80.0855, 80.1168, 80.1707, 80.2688, 80.4535, 80.8076, 81.5052, 82.9033, 86.0630, 92.0674, 105.3427]

        ax.plot(FLIOPCoefficientMPC.xs, baseline, color='red', marker='o', label=r'constant compression ($\lambda = 2$)')
        ax.plot(FLIOPCoefficientMPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoefficientMPC.xs, labels=FLIOPCoefficientMPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_wan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_portion():
        fig, ax = plt.subplots(figsize=(5, 4))

        bottom = np.zeros(6)
        width = 0.7

        labels = ["constant", "optimum"]
        groups = ["3pc", "6pc", "9pc"]
        color = ['tab:green', 'tab:cyan', 'tab:orange']

        times = {
            "communication time": np.array(
                [80.0968, 80.0960, 80.0968, 80.0944, 80.0968, 80.0960]),
            "prover time": np.array(
                [19.9454, 6.2265, 19.5302, 6.1122, 19.8861, 6.2482]),
            "verifier time": np.array(
                [5.3005, 1.6127, 5.2846, 1.6245, 5.2921, 1.5158]),
        }

        col_pos = np.array([0, 1, 2.5, 3.5, 5, 6])
        for idx, (label, element) in enumerate(times.items()):
            rects = ax.bar(col_pos, element, width, label=label, color=color[idx], bottom=bottom, edgecolor='black')
            if label != 'verifier time':
                ax.bar_label(rects, label_type="center", fmt=lambda x: '%.2f' % x, fontsize='small')
            else:
                i = 0
                for rect, ver_label in zip(rects, times['verifier time']):
                    ax.text(
                        rect.get_x() + rect.get_width() / 2, (bottom.item(i) + 5.5) if i % 2 == 0 else (bottom.item(i) + 1.5), '%.2f' % ver_label, ha="center", va="bottom", fontsize='small'
                    )
                    i += 1
            bottom += element

        plt.xticks(ticks=col_pos, labels=np.tile(labels, len(groups)), rotation=20)

        plt.xlabel('')
        col_pos = col_pos.reshape(-1, len(labels)).mean(axis=1)
        trans = transforms.blended_transform_factory(ax.transData, ax.transAxes)

        for pos, label in zip(col_pos, groups):
            ax.text(pos, -0.22, label, transform=trans, ha='center', va='bottom', fontsize=12)
        for pos in (col_pos[:-1] + col_pos[1:]) / 2:
            ax.axvline(pos, 0, -0.22, ls=':', color='black', clip_on=False)

        ax.set_ylim(0, 152)
        plt.ylabel(r'Total time of input length $2^{16}$ in WAN (ms)')
        plt.tight_layout()

        handles, labels = plt.gca().get_legend_handles_labels()
        order = [2, 1, 0]
        plt.legend(loc='upper left', handles=[handles[idx] for idx in order], labels=[labels[idx] for idx in order])
        plt.savefig(f'wan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_payload():
        fig, ax = plt.subplots(figsize=(5, 4))

        optimum = [312, 376, 400, 464, 544, 568, 616, 696, 736, 784, 864, 960]
        constant = [352, 408, 464, 520, 576, 632, 688, 744, 800, 856, 912, 968]

        ax.plot(FLIOP_MPC.xs, constant, color='red', marker='o', label='constant compression ($\lambda = 2$)')
        ax.plot(FLIOP_MPC.xs, optimum, color='blue', marker='^', label='optimal compression')

        plt.xlabel('Input vector length')
        plt.ylabel('Total payload size in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP_MPC.xs, labels=FLIOP_MPC.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_wan_payload.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOPCoefficientMPC.lan()
        FLIOPCoefficientMPC.lan_portion()
        FLIOPCoefficientMPC.lan_payload()
        FLIOPCoefficientMPC.wan()
        FLIOPCoefficientMPC.wan_portion()
        FLIOPCoefficientMPC.wan_payload()


class ComparisonOptimalSchedules:
    xs = [2 ** i for i in range(5, 17)]
    x_labels = [rf'$2^{{{i}}}$' for i in range(5, 17)]

    @staticmethod
    def lan():
        fig, ax = plt.subplots(figsize=(5, 4))

        coefficient = [0.0731, 0.0768, 0.0842, 0.1020, 0.1314, 0.1923, 0.3132, 0.5426, 1.0127, 1.9534, 3.8155, 7.8995]
        baseline = [0.0814, 0.0908, 0.1098, 0.1434, 0.2091, 0.3409, 0.5946, 1.1032, 2.1291, 4.1517, 8.4775, 16.9121]

        ax.plot(ComparisonOptimalSchedules.xs, baseline, color='red', marker='o', label='optimal baseline')
        ax.plot(ComparisonOptimalSchedules.xs, coefficient, color='blue', marker='^', label='optimal coefficient')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=ComparisonOptimalSchedules.xs, labels=ComparisonOptimalSchedules.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_lan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_portion():
        fig, ax = plt.subplots(figsize=(5, 4))

        bottom = np.zeros(6)
        width = 0.7

        labels = ["baseline", "coefficient"]
        groups = ["3pc", "6pc", "9pc"]
        color = ['tab:cyan', 'tab:orange']

        times = {
            "prover time": np.array(
                [10.2084, 6.2265, 10.5304, 6.1122, 10.4393, 6.2482]),
            "verifier time": np.array(
                [6.6434, 1.6127, 6.5573, 1.6245, 6.6928, 1.5158]),
        }

        col_pos = np.array([0, 1, 2.5, 3.5, 5, 6])
        for idx, (label, element) in enumerate(times.items()):
            rects = ax.bar(col_pos, element, width, label=label, color=color[idx], bottom=bottom, edgecolor='black')
            ax.bar_label(rects, label_type="center", fmt=lambda x: '%.2f' % x, fontsize='small')
            bottom += element

        plt.xticks(ticks=col_pos, labels=np.tile(labels, len(groups)), rotation=20)

        plt.xlabel('')
        col_pos = col_pos.reshape(-1, len(labels)).mean(axis=1)
        trans = transforms.blended_transform_factory(ax.transData, ax.transAxes)

        ax.text(6.65, 20.7, r'(communication time $\approx$ 0.06 ms)', fontsize=10, ha='right', va='top')
        for pos, label in zip(col_pos, groups):
            ax.text(pos, -0.22, label, transform=trans, ha='center', va='bottom', fontsize=12)
        for pos in (col_pos[:-1] + col_pos[1:]) / 2:
            ax.axvline(pos, 0, -0.22, ls=':', color='black', clip_on=False)

        ax.set_ylim(0, 21)
        plt.ylabel(r'Total time of input length $2^{16}$ in LAN (ms)')
        plt.tight_layout()

        handles, labels = plt.gca().get_legend_handles_labels()
        order = [1, 0]
        plt.legend(loc='upper left', handles=[handles[idx] for idx in order], labels=[labels[idx] for idx in order])
        plt.savefig(f'lan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_payload():
        fig, ax = plt.subplots(figsize=(5, 4))

        coefficient = [344, 392, 488, 648, 560, 640, 800, 712, 792, 904, 1096, 960]
        baseline = [336, 368, 448, 456, 520, 544, 592, 640, 680, 760, 824, 832]

        ax.plot(ComparisonOptimalSchedules.xs, coefficient, color='blue', marker='^', label='optimal coefficient')
        ax.plot(ComparisonOptimalSchedules.xs, baseline, color='red', marker='o', label='optimal baseline')

        plt.xlabel('Input vector length')
        plt.ylabel('Total payload size in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=ComparisonOptimalSchedules.xs, labels=ComparisonOptimalSchedules.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_lan_payload.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan():
        fig, ax = plt.subplots(figsize=(5, 4))

        coefficient = [80.0444, 80.0551, 80.0682, 80.0898, 80.1261, 80.1922, 80.3171, 80.5528, 81.0290, 81.9753, 83.8430, 87.9352]
        baseline = [80.0518, 80.0671, 80.0895, 80.1281, 80.1985, 80.3351, 80.5933, 81.1069, 82.1366, 84.1646, 88.4968, 96.9343]

        ax.plot(ComparisonOptimalSchedules.xs, baseline, color='red', marker='o',
                label=r'optimal baseline')
        ax.plot(ComparisonOptimalSchedules.xs, coefficient, color='blue', marker='^', label='optimal coefficient')

        plt.xlabel('Input vector length')
        plt.ylabel('Total time in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=ComparisonOptimalSchedules.xs, labels=ComparisonOptimalSchedules.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_wan_total.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_portion():
        fig, ax = plt.subplots(figsize=(5, 4))

        bottom = np.zeros(6)
        width = 0.7

        labels = ["baseline", "coefficient"]
        groups = ["3pc", "6pc", "9pc"]
        color = ['tab:green', 'tab:cyan', 'tab:orange']

        times = {
            "communication time": np.array(
                [80.0816, 80.0960, 80.0800, 80.0944, 80.0816, 80.0960]),
            "prover time": np.array(
                [10.2080, 6.2265, 10.5332, 6.1122, 10.4382, 6.2482]),
            "verifier time": np.array(
                [6.6447, 1.6127, 6.5572, 1.6245, 6.6949, 1.5158]),
        }

        col_pos = np.array([0, 1, 2.5, 3.5, 5, 6])
        for idx, (label, element) in enumerate(times.items()):
            rects = ax.bar(col_pos, element, width, label=label, color=color[idx], bottom=bottom, edgecolor='black')
            if label != 'verifier time':
                ax.bar_label(rects, label_type="center", fmt=lambda x: '%.2f' % x, fontsize='small')
            else:
                i = 0
                for rect, ver_label in zip(rects, times['verifier time']):
                    ax.text(
                        rect.get_x() + rect.get_width() / 2,
                        (bottom.item(i) + 7) if i % 2 == 0 else (bottom.item(i) + 2), '%.2f' % ver_label, ha="center",
                        va="bottom", fontsize='small'
                    )
                    i += 1
            bottom += element

        plt.xticks(ticks=col_pos, labels=np.tile(labels, len(groups)), rotation=20)

        plt.xlabel('')
        col_pos = col_pos.reshape(-1, len(labels)).mean(axis=1)
        trans = transforms.blended_transform_factory(ax.transData, ax.transAxes)

        for pos, label in zip(col_pos, groups):
            ax.text(pos, -0.22, label, transform=trans, ha='center', va='bottom', fontsize=12)
        for pos in (col_pos[:-1] + col_pos[1:]) / 2:
            ax.axvline(pos, 0, -0.22, ls=':', color='black', clip_on=False)

        ax.set_ylim(0, 140)
        plt.ylabel(r'Total time of input length $2^{16}$ in WAN (ms)')
        plt.tight_layout()

        handles, labels = plt.gca().get_legend_handles_labels()
        order = [2, 1, 0]
        plt.legend(loc='upper left', handles=[handles[idx] for idx in order], labels=[labels[idx] for idx in order])
        plt.savefig(f'wan_portion.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_payload():
        fig, ax = plt.subplots(figsize=(5, 4))

        coefficient = [312, 376, 400, 464, 544, 568, 616, 696, 736, 784, 864, 960]
        baseline = [304, 352, 392, 440, 488, 544, 576, 632, 664, 720, 792, 816]

        ax.plot(ComparisonOptimalSchedules.xs, coefficient, color='blue', marker='^', label='optimal coefficient')
        ax.plot(ComparisonOptimalSchedules.xs, baseline, color='red', marker='o', label='optimal baseline')

        plt.xlabel('Input vector length')
        plt.ylabel('Total payload size in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=ComparisonOptimalSchedules.xs, labels=ComparisonOptimalSchedules.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'3pc_wan_payload.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        ComparisonOptimalSchedules.lan()
        ComparisonOptimalSchedules.lan_portion()
        ComparisonOptimalSchedules.lan_payload()
        ComparisonOptimalSchedules.wan()
        ComparisonOptimalSchedules.wan_portion()
        ComparisonOptimalSchedules.wan_payload()


if __name__ == '__main__':
    FLIOPCoefficientMPC.plot()
