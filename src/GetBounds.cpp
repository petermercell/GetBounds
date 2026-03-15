// GetBounds - Michael Millspaugh
// www.tks-designs.com

#include "Bounds.h"

using namespace std;

static const char* const CLASS = "GetBounds"; // Name of the class (should be the same as the name of the final .dll file)

struct rowValues {
	int first, last;
};

typedef struct rowValues Struct;

void GetBounds::_validate(bool for_real) {
	copy_info(); // Output image will have the same properties as the input

	info_.black_outside(_blackOutside);

	Format format = input0().format();
	Box inputbounds = Box(format.x(), format.y(), format.r(), format.t());
	ImagePlane imagePlane = ImagePlane(inputbounds, true, channels);


	RequestOutput reqData = RequestOutput();
	getRequests(inputbounds, channels, 0, reqData);
	input0().request(channels, 0);

	Interest interest(input0(), inputbounds, imagePlane.channels(), true);
	interest.unlock();

	if (input0().real_valid() && !input0().inErrorState() && !input0().inInvalidState()) {
		if (input0().tryValidate() && interest.valid()) {
			//only do this if the input hash has changed
			running(true);
			getBounds(imagePlane, format, interest);

			Box bounds(p[0], p[1], p[2], p[3]);
			//dont do it if the box is zero or negative (not sure why thats happening)
			if (bounds.area() >= 1) {
				info_.set(bounds);
			};

			_lastHash = input0().hash();
			running(false);

			//Op::warning("Get Bounds set bounding box ");
		}
	}

	//Op::warning("Get Bounds validated ");

}

void GetBounds::getRequests(const Box& box, const ChannelSet& channelsSet, int count, RequestOutput& reqData) {
	return PlanarIop::getRequests(box, channelsSet, count, reqData);
}

void GetBounds::getBounds(ImagePlane& imagePlane, Format& format, Interest& interest) {
	const int bx = imagePlane.bounds().x();
	const int by = imagePlane.bounds().y();
	const int br = imagePlane.bounds().r();  // exclusive right
	const int bt = imagePlane.bounds().t();  // exclusive top

	bool yMinFound = false;
	bool xMinFound = false;
	bool xMaxFound = false;

	// for each channel
	foreach(z, channels) {
		int chanNo = imagePlane.chanNo(z);
		string chanName = getName(z);

		for (int y = by; y < bt; y++) {

			float maxValue = 0;

			// walk left to right to find xMin
			for (int x = bx; x < br; x++) {
				try {
					float currentPixel = interest.at(x, y, z);

					if (currentPixel > 0) {
						if (currentPixel > maxValue) maxValue = currentPixel;

						if (!xMinFound) {
							p[0] = x - _extra;
							xMinFound = true;
						}
						else {
							if (x - _extra < p[0]) { p[0] = x - _extra; }
						}
						break;
					}
				}
				catch (...) {
					Op::warning("Attempt to GetBounds before source is available");
					abort();
				};
			};

			// walk right to left to find xMax
			// br is exclusive, so last valid pixel is br-1
			for (int x = br; x-- > bx; ) {

				float currentPixel = interest.at(x, y, z);

				if (currentPixel > 0) {
					if (currentPixel > maxValue) maxValue = currentPixel;

					// +1 because Nuke Box r() is exclusive (one past last pixel)
					if (!xMaxFound) {
						p[2] = x + 1 + _extra;
						xMaxFound = true;
					}
					else {
						if (x + 1 + _extra > p[2]) { p[2] = x + 1 + _extra; }
					}
					break;
				};
			};

			// update yMin / yMax
			if (maxValue > 0) {
				if (!yMinFound) {
					p[1] = y - _extra;
					yMinFound = true;
				}
				// always update yMax to the latest row with content
				// +1 because Nuke Box t() is exclusive (one past last row)
				p[3] = y + 1 + _extra;
			};
		};
	};
}

void GetBounds::renderStripe(ImagePlane& imagePlane)
{
	input0().fetchPlane(imagePlane);
	imagePlane.makeUnique();

	//std::cerr << to_string(p[0]) + "," + to_string(p[1]) + "," + to_string(p[2]) + "," + to_string(p[3]) << std::endl;
}
void GetBounds::knobs(Knob_Callback f) {
	ChannelSet_knob(f, &channels, "channels");SetFlags(f, Knob::EARLY_STORE);
	Bool_knob(f, &_blackOutside, "black outside"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::ALWAYS_SAVE); SetFlags(f, Knob::EARLY_STORE);
	Bool_knob(f, &_ignoreNegative, "ignore <0"); SetFlags(f, Knob::EARLY_STORE);
	Int_knob(f, &_extra, "extra");
}

bool GetBounds::renderFullPlanes() {
	//When renderFullPlanes() returns true, the plugin will never be given an image plane to fill that contains less than the full set of channels.
	return true;
}


static Iop* build(Node* node) {
	return new GetBounds(node);
}
const PlanarIop::Description GetBounds::dGet("GetBounds", "GetBounds", build); // Nuke reads this to get the name of your plugin in the UI. Make sure this is the same as the name of the class and the final .dll file!